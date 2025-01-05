from enum import Enum
from pathlib import Path


class Paths:
    data = Path(__file__).parent.parent / "data"
    matrices = data / "matrices"
    subsets = data / "subsets"


class MatrixFormats(Enum):
    MATRIX_MARKET = "MM", "mtx", "tar.gz"
    HARWELL_BOEING = "RB", "rb", "tar.gz"  # Also known as Rutherford-Boeing
    MATLAB = "mat", "mat", "mat"

    def __new__(cls, id, ext_local, ext_remote):
        member = object.__new__(cls)
        member._value_ = id
        member.ext_local = ext_local
        member.ext_remote = ext_remote
        return member