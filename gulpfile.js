const gulp = require('gulp')
const exec = require('child_process').exec

gulp.task('node-gyp', gulp.series(function (cb) {
  exec('node-gyp build', function (err, stdout, stderr) {
    console.log(stdout)
    console.log(stderr)
    cb(err);
  })
}))

gulp.task('build', gulp.series('node-gyp', function () {
  gulp.watch('src/*', gulp.series('node-gyp'))
}))