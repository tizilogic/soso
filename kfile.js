let project = new Project('soso');

project.useAsLibrary = () => {
	project.cmd = false;
	project.debugDir = null;
	project.addExclude('tests/test_unit.c');
};

project.addFile('src/**');
project.addFile('tests/**');
if (typeof noShtPlease === 'undefined') {
    project.addFile('ext/sht/sht.c');
    project.addFile('ext/sht/murmur3.c');
	project.addIncludeDir('ext');
    global.noShtPlease = true;
}
project.addIncludeDir('src');
project.setCStd('c99');
project.setCppStd('c++11');
project.setDebugDir('bin');
project.setCmd();

project.flatten();
resolve(project);
