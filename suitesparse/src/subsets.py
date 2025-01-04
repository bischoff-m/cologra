import pandas as pd
from definitions import Paths

subset_functions = dict()


def subset(name: str):
    """
    Decorator to register a subset function with a given name. The function must
    take a DataFrame as input and return a DataFrame with the same columns.

    Parameters
    ----------
    name : str
        The name to register the subset function under.

    Raises
    ------
    ValueError
        If a subset function with the given name already exists or if the name
        is not a valid identifier.

    Returns
    -------
    function
        The decorator function.
    """

    def decorator(func):
        if name in subset_functions:
            raise ValueError(f"Subset function with name {name} already exists")
        if not name.isidentifier():
            raise ValueError(f"Subset name {name} is not a valid identifier")
        subset_functions[name] = func
        return func

    return decorator


def create_subsets(df: pd.DataFrame) -> pd.Series:
    """
    Create subsets of the given DataFrame and save them as JSON files.

    Parameters
    ----------
    df : pd.DataFrame
        The DataFrame to create subsets from.

    Returns
    -------
    pd.Series
        A boolean mask indicating which rows are included in any subset.
    """
    # Compute matrices that need to be downloaded, if not saved already
    if len(subset_functions) == 0:
        print("No subset functions defined")

    # Remove every subset_*.json file in Paths.subsets
    Paths.subsets.mkdir(exist_ok=True)
    for file in Paths.subsets.glob("subset_*.json"):
        file.unlink()

    # Call each subset function and create mask of selected matrices
    mask = pd.Series([False] * df.shape[0])
    df_subs: dict[str, pd.DataFrame] = dict()
    for name, func in subset_functions.items():
        df_subs[name] = func(df)
        mask |= df["id"].isin(df_subs[name]["id"])

    # Write index JSON file of downloaded matrices
    with open(Paths.subsets / "index.json", "w") as f:
        df_export = df[mask].copy()
        df_export.drop(columns=["downloaded"], inplace=True)
        df_export["filepath"] = df_export["filepath"].apply(
            lambda x: str(x.relative_to(Paths.matrices))
        )
        f.write(df_export.to_json(orient="records", indent=2))

    # Write each subset to a JSON file
    for name, df_sub in df_subs.items():
        with open(Paths.subsets / f"subset_{name}.json", "w") as f:
            print(f"Subset {name} contains {df_sub.shape[0]} matrices")
            f.write(df_sub["id"].to_json(orient="records", indent=2))

    return mask
