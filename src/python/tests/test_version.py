import kdmp_parser


def test_version():
    assert isinstance(kdmp_parser.version.major, int)
    assert isinstance(kdmp_parser.version.minor, int)
    assert isinstance(kdmp_parser.version.patch, int)
    assert isinstance(kdmp_parser.version.release, str)
