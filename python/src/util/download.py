from pathlib import Path
from urllib.request import urlretrieve

import pandas as pd
from util.definitions import Paths
from tqdm import tqdm
from shutil import unpack_archive


# From https://github.com/tqdm/tqdm#hooks-and-callbacks
class TqdmUpTo(tqdm):
    """Provides `update_to(n)` which uses `tqdm.update(delta_n)`."""

    def update_to(self, b=1, bsize=1, tsize=None):
        """
        b  : int, optional
            Number of blocks transferred so far [default: 1].
        bsize  : int, optional
            Size of each block (in tqdm units) [default: 1].
        tsize  : int, optional
            Total size (in tqdm units). If [default: None] remains unchanged.
        """
        if tsize is not None:
            self.total = tsize
        return self.update(b * bsize - self.n)  # also sets self.n = b * bsize


def download_matrix(
    url: str,
    filepath: Path,
    pbar: TqdmUpTo,
) -> None:
    """
    Download a matrix from a URL and save it to a file. If the file is a tar.gz
    archive, it will be extracted and replaced with the specified matrix file.

    Parameters
    ----------
    url : str
        URL of the matrix file.
    filepath : Path
        Path to save the downloaded file.
    pbar : TqdmUpTo
        Progress bar to update.
    """
    # Create parent directories if they don't exist
    filepath.parent.mkdir(parents=True, exist_ok=True)

    # Check if expecting a tar.gz file
    ext_remote = ".".join(url.split("/")[-1].split(".")[1:])
    if filepath.suffix != f".{ext_remote}":
        if ext_remote != "tar.gz":
            raise ValueError(f"Unexpected file extension {ext_remote}")
        actual_filepath = filepath.with_suffix(".tar.gz")
    else:
        actual_filepath = filepath

    urlretrieve(url, actual_filepath, reporthook=pbar.update_to)
    pbar.total = pbar.n

    # Extract tar.gz file if necessary
    if ext_remote == "tar.gz":
        unpack_archive(actual_filepath, filepath.parent)
        actual_filepath.unlink()
        # Move specified matrix file to parent directory and remove the rest
        extracted_dir = filepath.parent / actual_filepath.name.removesuffix(".tar.gz")
        for file in extracted_dir.iterdir():
            if file.name == filepath.name:
                file.rename(filepath.parent / file.name)
            else:
                file.unlink()
        extracted_dir.rmdir()


def download_matrices(df: pd.DataFrame) -> None:
    """
    Download matrices from a DataFrame containing the columns "url", "filepath",
    "group", "name", and "downloaded". The "downloaded" column is updated to
    True for matrices that are successfully downloaded.

    Parameters
    ----------
    df : pd.DataFrame
        DataFrame containing the columns "url", "filepath", "group", "name",
        and "downloaded".
    """
    n_matrices = df[~df["downloaded"]].shape[0]
    total_pbar = tqdm(total=n_matrices, desc="Total Progress", position=0)
    current_pbar = TqdmUpTo(unit="B", unit_scale=True, unit_divisor=1024, position=1)

    for row in df.itertuples():
        current_pbar.reset()
        current_pbar.set_description(f"{row.group}/{row.name}")
        download_matrix(row.url, row.filepath, current_pbar)
        total_pbar.update(1)

    total_pbar.close()
    current_pbar.close()


def remove_unused(df: pd.DataFrame, mask: pd.Series) -> None:
    """
    Ask the user to remove matrices that are downloaded but not selected. Also
    remove empty directories after removing matrices.

    Parameters
    ----------
    df : pd.DataFrame
        DataFrame containing the columns "filepath" and "downloaded".
    mask : pd.Series
        Boolean mask indicating which matrices are selected.
    """
    to_remove = df[~mask & df["downloaded"]]["filepath"]
    if to_remove.empty:
        return

    answer = input(f"Remove {len(to_remove)} matrices? [y/n] ")
    if answer.lower() == "y":
        for path in to_remove:
            path.unlink()
            if not any(path.parent.iterdir()):
                path.parent.rmdir()
        for dir in Paths.matrices.iterdir():
            if not any(dir.iterdir()):
                dir.rmdir()

        print(f"Removed {len(to_remove)} matrices")
    else:
        print("No matrices removed")
