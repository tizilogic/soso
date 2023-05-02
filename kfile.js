let project = new Project('soso');

project.useAsLibrary = () => {
	project.cmd = false;
	project.debugDir = null;
};

project.addFile('src/**');
project.addFile('tests/**');
project.addFile('ext/**');
project.addExclude('ext/sht/example.c');
project.addIncludeDir('src');
project.addIncludeDir('ext');
project.setCStd('c99');
project.setCppStd('c++11');
project.setDebugDir('bin');
project.setCmd();

project.flatten();
resolve(project);
