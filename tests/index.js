const { MagickNode } = require('../build/Debug/NodeImageMagick')
const fs = require('fs')
const path = require('path');

(async function () {
  let img = new MagickNode('img/dim-sum440x280.png')
  img.crop(10, 10).then(() => console.log('done!')).catch(err => console.log('error!'))
})()
console.log('Starting process...')