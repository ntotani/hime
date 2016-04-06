{
  "targets": [
    {
      "target_name": "hime",
      "sources": [
          "hime.cc",
          "session_wrap.cc",
          "../../src/board.cpp",
          "../../src/master.cpp",
          "../../src/session.cpp"
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "../../include"
      ],
      'xcode_settings': {
          'MACOSX_DEPLOYMENT_TARGET': '10.8'
      }
    }
  ]
}
