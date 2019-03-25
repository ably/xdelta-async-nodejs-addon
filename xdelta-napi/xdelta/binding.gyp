{
  'targets': [
    {
      'target_name': 'Xdelta3Addon',
      'sources': [ 'Xdelta3Addon.cc', 'XdeltaEncodeAsyncWorker.cc', 'XdeltaDecodeAsyncWorker.cc', 'BaseXdeltaAsyncWorker.cc' ],
      'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")", "./", "./xdelta-3.1.0/xdelta3"],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      "libraries": [ "../xdelta-3.1.0/xdelta3/libxdelta3.a" ],
      "cflags": [ "-DHAVE_CONFIG_H" ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
    }
  ]
}