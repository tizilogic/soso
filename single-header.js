const fs = require('fs');

let header = '';
let src_top = '#include "soso.h"\n\n';
let src = '';

log.info('Generating header...');
try {
    const data = fs.readFileSync(__dirname + '/src/soso/soso.h', {encoding: 'utf8'}).replace('#include <sht/sht.h>', '');
    let cut = data.indexOf('//--- Internals ---//');
    header += data.slice(0, cut);
    src_top += data.slice(cut) + '\n\n';
} catch (err) {
    throw new Error(err);
}

try {
    fs.writeFileSync(__dirname + '/soso.h', header, {flag: 'w'});
} catch (err) {
    throw new Error(err);
}

log.info('Generating compile unit...');
try {
    const data = fs.readFileSync(__dirname + '/ext/sht/murmur3.h', {encoding: 'utf8'});
    src_top += data + '\n\n';
} catch (err) {
    throw new Error(err);
}
try {
    const data = fs.readFileSync(__dirname + '/ext/sht/murmur3.c', {encoding: 'utf8'}).replace('#include "murmur3.h"', '');
    src += data + '\n\n';
} catch (err) {
    throw new Error(err);
}

try {
    const data = fs.readFileSync(__dirname + '/ext/sht/sht.h', {encoding: 'utf8'}).replace('#pragma once', '');
    src_top += data + '\n\n';
} catch (err) {
    throw new Error(err);
}
try {
    const data = fs.readFileSync(__dirname + '/ext/sht/sht.c', {encoding: 'utf8'}).replace('#include "murmur3.h"', '').replace('#include "sht.h"', '');
    src += data + '\n\n';
} catch (err) {
    throw new Error(err);
}

try {
    const data = fs.readFileSync(__dirname + '/src/soso/random.c', {encoding: 'utf8'}).replace('#include "soso.h"', '');
    src += data + '\n\n';
} catch (err) {
    throw new Error(err);
}

try {
    const data = fs.readFileSync(__dirname + '/src/soso/soso.c', {encoding: 'utf8'}).replace('#include <sht/murmur3.h>', '').replace('#include "soso.h"', '');
    src += data;
} catch (err) {
    throw new Error(err);
}

try {
    fs.writeFileSync(__dirname + '/soso.c', src_top + '\n' + src, {flag: 'w'});
} catch (err) {
    throw new Error(err);
}

log.info('Done');

// Need empty project to not make kmake fail
let project = new Project('ThisIsNotAProject');
resolve(project);
