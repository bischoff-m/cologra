"""
This file contains definitions for which matrices to download.

Every function decorated with @subset(name: str) will be called to get a subset
of the available matrices. The function should take as input a pandas DataFrame
which contains all the matrices from the SuiteSparse Matrix Collection. Every
matrix is contained multiple times in the DataFrame, once for every format it
is available in.

For each subset function, a JSON file will be saved in the directory
Paths.subsets. This file will contain information about the matrices in the
subset, such as the group, name, and format.

On 2024-12-21, the DataFrame contains 2904 matrices and the following columns.
You can get the DataFrame by calling index_available().

 #   Column      Non-Null Count  Dtype
---  ------      --------------  -----
 0   group       8712 non-null   object
 1   name        8712 non-null   object
 2   nrows       8712 non-null   int64
 3   ncols       8712 non-null   int64
 4   nnz         8712 non-null   int64
 5   isReal      8712 non-null   bool
 6   isBinary    8712 non-null   bool
 7   isND        8712 non-null   bool
 8   posdef      8712 non-null   bool
 9   psym        8712 non-null   bool
 10  nsym        8712 non-null   bool
 11  kind        8712 non-null   object
 12  nentries    8712 non-null   int64
 13  format      8712 non-null   object
 14  id          8712 non-null   object
 15  filepath    8712 non-null   object
 16  downloaded  8712 non-null   bool
 17  url         8712 non-null   object

The DataFrame is derived from https://sparse.tamu.edu/files/ssstats.csv and was
formerly called UFstats.csv. Find documentation about the structure here:
https://sparse.tamu.edu/statistics
"""

import pandas as pd
from suitesparse.util.definitions import MatrixFormats
from suitesparse.util.ss_index import index_available
from suitesparse.util.subsets import subset


@subset("small_mtx")
def subset_small_mtx(df: pd.DataFrame) -> pd.DataFrame:
    df = df[df["format"] == MatrixFormats.MATRIX_MARKET.value]
    df = df[df["nnz"] < 1e4]
    df = df[df["isReal"]]
    return df.sample(n=100, random_state=42)


@subset("small_mtx_3elements")
def subset_small_mtx_3elements(df: pd.DataFrame) -> pd.DataFrame:
    df = df[df["format"] == MatrixFormats.MATRIX_MARKET.value]
    df = df[df["nnz"] < 1e4]
    df = df[df["isReal"]]
    return df.sample(n=3, random_state=42)


@subset("for_testing")
def subset_for_testing(df: pd.DataFrame) -> pd.DataFrame:
    df = df[df["format"] == MatrixFormats.MATRIX_MARKET.value]
    df = df[df["group"] == "AG-Monien"]
    df = df[df["name"] == "netz4504"]
    assert len(df) == 1
    return df


@subset("medium_mtx")
def subset_medium_mtx(df: pd.DataFrame) -> pd.DataFrame:
    df = df[df["format"] == MatrixFormats.MATRIX_MARKET.value]
    df = df[(df["nnz"] < 2e4) & (df["nnz"] > 1e4)]
    df = df[df["isReal"]]
    return df.sample(n=5, random_state=42)


@subset("only_netz4504")
def subset_only_netz4504(df: pd.DataFrame) -> pd.DataFrame:
    df = df[df["format"] == MatrixFormats.MATRIX_MARKET.value]
    df = df[df["group"] == "AG-Monien"]
    df = df[df["name"] == "netz4504"]
    assert len(df) == 1
    return df


@subset("small_square")
def subset_small_square(df: pd.DataFrame) -> pd.DataFrame:
    df = df[df["format"] == MatrixFormats.MATRIX_MARKET.value]
    df = df[df["nrows"] == df["ncols"]]
    df = df[df["nnz"] < 1e4]
    df = df[df["isReal"]]
    return df.sample(n=100, random_state=42)


@subset("medium_square")
def subset_medium_square(df: pd.DataFrame) -> pd.DataFrame:
    df = df[df["format"] == MatrixFormats.MATRIX_MARKET.value]
    df = df[df["nrows"] == df["ncols"]]
    df = df[(df["nnz"] < 1e5) & (df["nnz"] > 1e4)]
    df = df[df["isReal"]]
    return df.sample(n=100, random_state=42)


@subset("large_square")
def subset_large_square(df: pd.DataFrame) -> pd.DataFrame:
    df = df[df["format"] == MatrixFormats.MATRIX_MARKET.value]
    df = df[df["nrows"] == df["ncols"]]
    df = df[(df["nnz"] < 1e6) & (df["nnz"] > 1e5)]
    df = df[df["isReal"]]
    return df.sample(n=10, random_state=42)


@subset("many_small")
def subset_many_small(df: pd.DataFrame) -> pd.DataFrame:
    df = df[df["format"] == MatrixFormats.MATRIX_MARKET.value]
    df = df[df["nnz"] < 2e4]
    df = df[df["isReal"]]
    return df


if __name__ == "__main__":
    df = index_available()
    print(df.info())
