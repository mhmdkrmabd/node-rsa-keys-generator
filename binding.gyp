{
  "targets": [
    {
      "target_name": "keys_generator",
      "sources": [
        "src/napi_wrapper.cpp",
        "src/platform_utils.cpp",
        "src/keyring.cpp",
        "src/rsa_generator.cpp"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "src/",
        "<(node_root_dir)/include/node"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "cflags!": ["-fno-exceptions"],
      "cflags_cc!": ["-fno-exceptions"],
      "defines": ["NAPI_DISABLE_CPP_EXCEPTIONS"],
      "conditions": [
        [
          "OS=='win'",
          {
            "defines": ["WINDOWS_PLATFORM"],
            "include_dirs": [
              "<(module_root_dir)/deps/openssl/include"
            ],
            "library_dirs": [
              "<(module_root_dir)/deps/openssl/lib"
            ],
            "libraries": [
              "-ladvapi32",
              "-lcrypt32",
              "-llibssl",
              "-llibcrypto"
            ],
            "msvs_settings": {
              "VCCLCompilerTool": {
                "ExceptionHandling": 1
              }
            }
          }
        ],
        [
          "OS=='linux'",
          {
            "defines": ["LINUX_PLATFORM"],
            "cflags": [
              "<!@(pkg-config --cflags openssl)",
              "<!@(pkg-config --cflags libsecret-1 || echo '')"
            ],
            "libraries": [
              "<!@(pkg-config --libs openssl)",
              "<!@(pkg-config --libs libsecret-1 || echo '')"
            ],
            "conditions": [
              [
                "<!(pkg-config --exists libsecret-1 >/dev/null 2>&1 && echo 1 || echo 0)==1",
                {
                  "defines": ["HAVE_LIBSECRET"]
                }
              ]
            ]
          }
        ],
        [
          "OS=='mac'",
          {
            "defines": ["MACOS_PLATFORM"],
            "include_dirs": [
              "<!@(brew --prefix openssl@3 2>/dev/null || echo /opt/homebrew/opt/openssl@3)/include",
              "/opt/homebrew/opt/openssl@3/include",
              "/usr/local/opt/openssl@3/include"
            ],
            "library_dirs": [
              "<!@(brew --prefix openssl@3 2>/dev/null || echo /opt/homebrew/opt/openssl@3)/lib",
              "/opt/homebrew/opt/openssl@3/lib",
              "/usr/local/opt/openssl@3/lib"
            ],
            "libraries": [
              "-framework Security",
              "-lssl",
              "-lcrypto"
            ],
            "xcode_settings": {
              "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
              "CLANG_CXX_LIBRARY": "libc++",
              "MACOSX_DEPLOYMENT_TARGET": "10.15"
            }
          }
        ]
      ]
    }
  ]
}