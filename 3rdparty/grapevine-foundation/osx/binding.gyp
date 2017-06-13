{
  "targets": [
    {
      "target_name": "grapevine",
      "cflags!": [ "-fno-exceptions", "-fno-rtti" ],
      "cflags_cc!": [ "-fno-exceptions", "-fno-rtti" ],
      "include_dirs": ["../include", "<!@(pkg-config sigc++-2.0 --cflags-only-I | sed s/-I//g)"],
      "sources": [ "objects/Grapevine_wrap.cpp" ],
      "conditions": [
          ["OS=='mac'", {
              "xcode_settings": {
                  "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
                  "GCC_ENABLE_CPP_RTTI": "YES"
              }
          }]
      ],

     "link_settings": {
          "libraries": [
              "-L./.. -lgrapevine"
          ]
      }

    }
  ]
}