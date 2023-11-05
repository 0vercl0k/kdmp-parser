from ._kdmp_parser import (  # type: ignore
    PageSize as size,
    PageAlign as align,
    PageOffset as offset,
)

VALID_PAGE_SIZES = (0x1000, 0x20_0000, 0x4000_0000)
