"use strict";

module.exports = function (grunt) {

	grunt.loadNpmTasks('grunt-bump');
	grunt.loadNpmTasks('grunt-exec');

	grunt.initConfig({
		bump: {
			options: {
				files: ['package.json'],
				commit: true,
				commitMessage: 'Release version %VERSION%',
				commitFiles: ['package.json'],
				createTag: true,
				tagName: '%VERSION%',
				tagMessage: 'Version %VERSION%',
				push: false,
				prereleaseName: 'beta'
			}
		},
		exec: {
			ts: {
				command: 'node_modules/typescript/bin/tsc'
			}
		}
	});

	function execExternal(cmd) {
		return function() {
			var done = this.async();
			grunt.log.ok("Executing " + cmd);
			require('child_process').exec(cmd, function(err, stdout, stderr) {
				if (err) {
					grunt.fatal('Error executing "' + cmd + '": ' + stderr);
				}
				console.log(stdout);
				stderr && console.error(stderr);
				done();
			});
		};
	}

	grunt.registerTask('release',
		'Runs TypeScript compilation, increments the version and makes a tagged commit. Run as "grunt release:type", where "type" is "major", "minor", "patch", "prepatch", etc.)',
		versionType => {
			grunt.task.run([
				'bump:' + versionType,
				'exec:ts'
			]);
		}
	);

	grunt.registerTask('release:git-push', 'Pushes to git', execExternal('git push origin master --follow-tags'));

	grunt.registerTask('release:npm-publish', 'Deploys to npm', execExternal('npm publish .'));

	grunt.registerTask('release:deploy', 'Pushes a new release to github and deploys to npm', [
		'release:git-push',
		'release:npm-publish'
	]);
};
