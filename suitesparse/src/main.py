import humanize

# Import targets file to let the decorator @subset register the functions. This
# must be done before importing the subset_functions variable.
import targets  # noqa: F401
from download import download_matrices, remove_unused
from ss_index import index_available
from subsets import create_subsets


def main():
    """
    Main function to download and update the SuiteSparse Matrix Collection.
    Which matrices to download is determined by the subsets defined in the
    targets.py file.
    """

    df = index_available()
    mask = create_subsets(df)
    print(f"Selected {mask.sum()} matrices out of {df.shape[0]}")

    # Ask to remove matrices that are downloaded but not selected
    remove_unused(df, mask)

    # Download missing matrices
    to_download = df[mask & ~df["downloaded"]]

    if not to_download.empty:
        nonzero = to_download["nnz"].sum()
        # Roughly 3 bytes per nonzero element and 100 KB overhead per matrix
        size_estimate = nonzero * 3 + to_download.shape[0] * 1e5
        print(
            f"Downloading {to_download.shape[0]} matrices with"
            f" ~{humanize.naturalsize(size_estimate)} in total (very rough estimate)"
        )
        download_matrices(to_download)

    print("Archive is up to date.")


if __name__ == "__main__":
    main()
