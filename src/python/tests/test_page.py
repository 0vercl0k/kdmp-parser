import kdmp_parser.page


def test_page():
    assert callable(kdmp_parser.page.align)
    assert callable(kdmp_parser.page.offset)
    assert isinstance(kdmp_parser.page.size, int)
    assert kdmp_parser.page.size in kdmp_parser.page.VALID_PAGE_SIZES
