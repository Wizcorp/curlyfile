{
  "make_global_settings": [
    ["CXX", "/usr/bin/clang++"],
    ["LINK", "/usr/bin/clang++"],
    ],
    "targets": [{
      "target_name": "curlyfile",
      "sources": [
        "src/downloader.cc",
        "src/curlyfile.cc"
        ],
      "include_dirs" : [
        "<!(node -e \"require('nan')\")"
        ],
      "libraries": [
        "-lcurl"
        ],
      "cppflags": [
        "-O3",
        "-std=c++0x",
        "-pthread"
        ],
      "conditions": [
        ["OS==\"mac\"", {
          "xcode_settings": {
            "OTHER_CPLUSPLUSFLAGS" : [
              "-O3",
              "-std=c++0x",
              "-stdlib=libc++",
              "-pthread",
              ],
            "OTHER_LDFLAGS": [
              "-stdlib=libc++"
              ]
          }
        }]
      ]
    }]
}
