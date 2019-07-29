# xdelta-async-nodejs-addon

[![Build Status](https://travis-ci.org/ably/xdelta-async-nodejs-addon.svg?branch=master)](https://travis-ci.org/ably/xdelta-async-nodejs-addon)

Asynchronous native addon for Node.js that allows generating and applying diffs/patches in VCDIFF format using the [xdelta](https://github.com/jmacd/xdelta) algorithm.

# Usage

The repo could be imported as npm package by adding 

```
"dependencies":  {
  ...
  "xdelta": "git://github.com/ably/xdelta-async-nodejs-addon.git",
  ...
}
```
to your package.json.

# Caveats

The build infrastructure of the addon currently depends on tools that are Mac and Linux specific. Consequently, an installation on Windows will not be possible at this time.

# Release checklist

1. Run `grunt release:<type>` (where `type` is "major", "minor", "patch", "prepatch")
2. Run `grunt release:deploy`
3. Visit [https://github.com/ably/xdelta-async-nodejs-addon/tags](https://github.com/ably/xdelta-async-nodejs-addon/tags) and draft new release for the newly created tag

# License

Copyright (c) 2019 Ably Real-time Ltd, Licensed under the Apache License, Version 2.0.  Refer to [LICENSE](LICENSE) for the license terms.
