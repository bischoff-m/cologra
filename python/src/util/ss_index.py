import re
from datetime import datetime
from urllib.request import urlopen

import pandas as pd
from util.definitions import MatrixFormats, Paths

SS_URL_BASE = "https://sparse.tamu.edu"
SS_URL_STATS = f"{SS_URL_BASE}/files/ssstats.csv"


def get_ssstats() -> str:
    """
    Download the ssstats.csv file from the SuiteSparse Matrix Collection.
    If the file is older than 4 weeks, it will be updated.

    Returns
    -------
    str
        Content of the ssstats.csv file.
    """
    filepath = Paths.data / "ssstats.csv"
    if filepath.exists():
        date_modified = datetime.fromtimestamp(filepath.lstat().st_mtime)
        if datetime.now() - date_modified < pd.Timedelta(weeks=4):
            return filepath.read_text()
        print(f"Updating {SS_URL_STATS} because it is older than 4 weeks")
    else:
        print(f"Downloading {SS_URL_STATS}")
    with urlopen(SS_URL_STATS) as f:
        data = f.read().decode("utf-8")

    filepath.write_text(data)
    return data


def index_available() -> pd.DataFrame:
    """
    Get the index of available matrices in the SuiteSparse Matrix Collection.

    Returns
    -------
    pd.DataFrame
        Index of available matrices.
    """
    lines = get_ssstats().splitlines()

    # Validate first line is number of matrices
    if not re.match(r"\d+", lines[0]):
        raise ValueError(
            "File ssstats.csv does not have the expected format. "
            "The first line should be the number of matrices."
        )

    # Validate second line is LastRevisionDate (like 31-Oct-2023 18:12:37)
    try:
        datetime.strptime(lines[1], "%d-%b-%Y %H:%M:%S")
    except ValueError:
        raise ValueError(
            "File ssstats.csv does not have the expected format. "
            "The second line should be the LastRevisionDate."
        )

    # Format of ssstats.csv explained here https://sparse.tamu.edu/statistics and
    # used here
    # https://github.com/jlblancoc/suitesparse-metis-for-windows/blob/master/SuiteSparse/ssget/ssgui.java

    df = pd.read_csv(Paths.data / "ssstats.csv", skiprows=2, header=None)
    if df.shape[1] != 13:
        raise ValueError(
            "File ssstats.csv does not have the expected format. "
            "It should have 13 columns."
        )

    df.columns = [
        "group",
        "name",
        "nrows",
        "ncols",
        "nnz",
        "isReal",
        "isBinary",
        "isND",
        "posdef",
        "psym",
        "nsym",
        "kind",
        "nentries",
    ]
    # Convert boolean columns
    for col in ["isReal", "isBinary", "isND", "posdef", "psym", "nsym"]:
        df[col] = df[col].astype(bool)

    # Duplicate dataframe for each matrix format
    dfs = []
    for fmt in MatrixFormats:
        df_fmt = df.copy()
        df_fmt["format"] = fmt.value
        dfs.append(df_fmt)
    df = pd.concat(dfs, ignore_index=True)

    # Set ID column to "format/group/name"
    df["id"] = df["format"] + "/" + df["group"] + "/" + df["name"]

    # Add column with local file path
    df["filepath"] = [
        Paths.matrices / row.group / f"{row.name}.{MatrixFormats(row.format).ext_local}"
        for row in df.itertuples()
    ]
    # Add column that indicates if the matrix is downloaded
    df["downloaded"] = df["filepath"].apply(lambda p: p.exists())
    # Add column with URL to download the matrix
    df["url"] = [
        f"{SS_URL_BASE}/{row.format}/{row.group}/{row.name}.{MatrixFormats(row.format).ext_remote}"
        for row in df.itertuples()
    ]
    return df
