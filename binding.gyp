{
  'targets': [
    {
      'target_name': 'Xdelta3Addon',
      'sources': [ 'src/Xdelta3Addon.cc', 'src/XdeltaEncodeAsyncWorker.cc', 'src/XdeltaDecodeAsyncWorker.cc', 'src/BaseXdeltaAsyncWorker.cc' ],
      'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")", "./src", "./xdelta-3.1.0/xdelta3"],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      "libraries": [ "../xdelta-3.1.0/xdelta3/libxdelta3.a" ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS', 'HAVE_CONFIG_H' ]
    }
  ]
}