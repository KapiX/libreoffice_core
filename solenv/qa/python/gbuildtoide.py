# -*- tab-width: 4; indent-tabs-mode: nil; py-indent-offset: 4 -*-
'''
  This file is part of the LibreOffice project.

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.

'''

import subprocess
import unittest
import json
import os
import os.path
import tempfile
import shutil


# these contortions seem unavoidable for windows
def getgbuildtesttools(testcase):
    with open(os.path.join(os.environ['WORKDIR'], 'CustomTarget', 'solenv', 'gbuildtesttools.done'), 'r') as f:
        gbuildtesttools = json.load(f)
    testcase.make = gbuildtesttools['MAKE']
    testcase.bash = gbuildtesttools['BASH']
    testcase.gbuildtojson = gbuildtesttools['GBUILDTOJSON']

makeenvvars = ['MAKEOVERRIDES', 'MAKEFLAGS', 'MAKE_TERMERR', 'MAKE_TERMOUT', 'MAKELEVEL', 'MFLAGS']
def clearmakevars():
    if 'LD_LIBRARY_PATH' in os.environ:
        os.environ['GBUILDTOJSON_LD_LIBRARY_PATH'] = os.environ['LD_LIBRARY_PATH']
        del(os.environ['LD_LIBRARY_PATH']) # built with ASAN; prevent that
    for makeenvvar in makeenvvars:
        if makeenvvar in os.environ:
            del os.environ[makeenvvar]


class CheckGbuildToIde(unittest.TestCase):
    def setUp(self):
        getgbuildtesttools(self)
        clearmakevars()
        self.tempwork = tempfile.mkdtemp()

    def tearDown(self):
        shutil.rmtree(self.tempwork)

    def test_gbuildtoide(self):
        os.makedirs(os.path.join(self.tempwork, 'LinkTarget', 'Executable'))
        shutil.copy(self.gbuildtojson, os.path.join(self.tempwork, 'LinkTarget', 'Executable'))
        (bashscripthandle, bashscriptname) = tempfile.mkstemp()
        bashscript = os.fdopen(bashscripthandle, 'w', newline='\n')
        bashscript.write("set -e\n")
        bashscript.write("cd %s/solenv/qa/python/selftest\n" % os.environ['SRCDIR'])
        bashscript.write("%s gbuildtoide WORKDIR=%s\n" % (self.make, self.tempwork.replace('\\', '/')))
        bashscript.close()
        subprocess.check_call([self.bash, bashscriptname.replace('\\', '/')])
        os.remove(bashscriptname)
        jsonfiles = os.listdir(os.path.join(self.tempwork, 'GbuildToIde', 'Library'))
        gbuildlibs = []
        for jsonfilename in jsonfiles:
            with open(os.path.join(self.tempwork, 'GbuildToIde', 'Library', jsonfilename), 'r') as f:
                gbuildlibs.append(json.load(f))
        foundlibs = set()
        for lib in gbuildlibs:
            self.assertEqual(set(lib.keys()), set(['ASMOBJECTS', 'CFLAGS', 'CFLAGSAPPEND', 'CXXFLAGS', 'CXXFLAGSAPPEND', 'CXXOBJECTS', 'DEFS', 'GENCOBJECTS', 'GENCXXOBJECTS', 'ILIBTARGET', 'INCLUDE', 'LINKED_LIBS', 'LINKED_STATIC_LIBS', 'LINKTARGET', 'OBJCFLAGS', 'OBJCFLAGSAPPEND', 'OBJCOBJECTS', 'OBJCXXFLAGS', 'OBJCXXFLAGSAPPEND', 'OBJCXXOBJECTS', 'YACCOBJECTS']))
            if lib['LINKTARGET'].find('gbuildselftestdep') != -1:
                foundlibs.add('gbuildselftestdep')
            elif lib['LINKTARGET'].find('gbuildselftest') != -1:
                foundlibs.add('gbuildselftest')
                self.assertIn('-Igbuildtoidetestinclude', lib['INCLUDE'].split())
                self.assertIn('gbuildselftestdep', lib['LINKED_LIBS'].split())
                self.assertIn('solenv/qa/python/selftest/selftestobject', lib['CXXOBJECTS'].split())
                self.assertIn('-DGBUILDSELFTESTDEF', lib['DEFS'].split())
                self.assertIn('-DGBUILDSELFTESTCXXFLAG', lib['CXXFLAGSAPPEND'].split())
                self.assertIn('-DGBUILDSELFTESTCFLAG', lib['CFLAGSAPPEND'].split())
            else:
                self.assertTrue(False)
        self.assertEqual(foundlibs, set(['gbuildselftest', 'gbuildselftestdep']))
        self.assertEqual(len(foundlibs), 2)
        jsonfiles = os.listdir(os.path.join(self.tempwork, 'GbuildToIde', 'Executable'))
        gbuildexes = []
        for jsonfilename in jsonfiles:
            with open(os.path.join(self.tempwork, 'GbuildToIde', 'Executable', jsonfilename), 'r') as f:
                gbuildexes.append(json.load(f))
        foundexes = set()
        for exe in gbuildexes:
            if exe['LINKTARGET'].find('gbuildselftestexe') != -1:
                foundexes.add('gbuildselftestexe')
            else:
                self.assertTrue(False)
        self.assertEqual(foundexes, set(['gbuildselftestexe']))
        self.assertEqual(len(foundexes), 1)

class CheckGbuildToIdeModules(unittest.TestCase):
    def setUp(self):
        getgbuildtesttools(self)
        clearmakevars()
        self.tempwork = tempfile.mkdtemp()
        self.tempsrc = tempfile.mkdtemp()
        self.srcdir = os.environ['SRCDIR']
        if os.environ['OS'] == 'WNT':
            self.tempworkmixed = self.tempwork.replace('\\','/')
            self.tempsrcmixed = self.tempsrc.replace('\\','/')
            self.srcdirnative = self.srcdir.replace('/','\\')
        else:
            self.tempworkmixed = self.tempwork
            self.tempsrcmixed = self.tempsrc
            self.srcdirnative = self.srcdir
        shutil.copyfile(os.path.join(self.srcdirnative, 'config_host.mk'), os.path.join(self.tempsrc, 'config_host.mk'))
        shutil.copyfile(os.path.join(self.srcdirnative, 'config_host_lang.mk'), os.path.join(self.tempsrc, 'config_host_lang.mk'))
        shutil.copyfile(os.path.join(self.srcdirnative, 'Repository.mk'), os.path.join(self.tempsrc, 'Repository.mk'))
        shutil.copyfile(os.path.join(self.srcdirnative, 'RepositoryExternal.mk'), os.path.join(self.tempsrc, 'RepositoryExternal.mk'))
        shutil.copyfile(os.path.join(self.srcdirnative, 'RepositoryFixes.mk'), os.path.join(self.tempsrc, 'RepositoryFixes.mk'))
        shutil.copytree(os.path.join(self.srcdirnative, 'config_host'),  os.path.join(self.tempsrc, 'config_host'))
        print('copytree from _%s_ to _%s_' % (os.path.join(self.srcdirnative, 'solenv').replace('\\', '#').replace('/', '!'), os.path.join(self.tempsrc, 'solenv').replace('\\', '#').replace('/', '!')))
        shutil.copytree(os.path.join(self.srcdirnative, 'solenv'),  os.path.join(self.tempsrc, 'solenv'))

    def tearDown(self):
        shutil.rmtree(self.tempsrc)
        shutil.rmtree(self.tempwork)

    def test_gbuildtoide(self):
        blacklisted_modules = ['compilerplugins', 'cli_ure', 'dictionaries', 'bridges', 'helpcompiler', 'helpcontent2', 'icon-themes', 'sal', 'shell', 'cppu', 'cppuhelper', 'cpputools', 'extensions', 'external', 'i18npool', 'javaunohelper', 'jurt', 'lingucomponent', 'odk', 'scaddins', 'solenv', 'stoc', 'tools', 'translations', 'writerfilter']
        modules = ['accessibility', 'android', 'animations', 'apple_remote', 'avmedia', 'basctl', 'basegfx', 'basic', 'bean', 'canvas', 'chart2', 'codemaker', 'comphelper', 'cppcanvas', 'cui', 'dbaccess', 'desktop', 'drawinglayer', 'dtrans', 'editeng', 'embeddedobj', 'embedserv', 'eventattacher', 'extras', 'filter', 'forms', 'formula', 'fpicker', 'framework', 'hwpfilter', 'i18nlangtag', 'i18nutil', 'idl', 'idlc', 'instsetoo_native', 'io', 'ios', 'jvmaccess', 'jvmfwk', 'l10ntools', 'librelogo', 'libreofficekit', 'linguistic', 'lotuswordpro', 'mysqlc', 'nlpsolver', 'o3tl', 'offapi', 'officecfg', 'onlineupdate', 'oovbaapi', 'oox', 'opencl', 'package', 'postprocess', 'pyuno', 'registry', 'remotebridges', 'reportbuilder', 'reportdesign', 'ridljar', 'rsc', 'salhelper', 'sax', 'sc', 'sccomp', 'scp2', 'scripting', 'sd', 'sdext', 'setup_native', 'sfx2', 'slideshow', 'smoketest', 'soltools', 'sot', 'starmath', 'store', 'svgio', 'svl', 'svtools', 'svx', 'sw', 'swext', 'sysui', 'test', 'testtools', 'toolkit', 'tubes', 'ucb', 'ucbhelper', 'udkapi', 'uitest', 'UnoControls', 'unodevtools', 'unoidl', 'unoil', 'unotest', 'unotools', 'unoxml', 'ure', 'uui', 'vbahelper', 'vcl', 'winaccessibility', 'wizards', 'writerperfect', 'xmerge', 'xmlhelp', 'xmloff', 'xmlreader', 'xmlscript', 'xmlsecurity']
        if os.environ['OS'] == 'WNT':
            # for now, use a limited subset for testing on windows as it is so annoyingly slow on this
            modules = ['chart2', 'cui', 'dbaccess', 'framework', 'oox', 'sfx2', 'svl', 'svtools', 'svx', 'toolkit', 'vcl', 'xmloff']
        for module in modules:
            shutil.rmtree(self.tempwork)
            os.makedirs(os.path.join(self.tempwork, 'LinkTarget', 'Executable'))
            shutil.copy(self.gbuildtojson, os.path.join(self.tempwork, 'LinkTarget', 'Executable'))
            if module != 'solenv':
                shutil.copytree(os.path.join(os.environ['SRCDIR'], module), os.path.join(self.tempsrc, module))
            (bashscripthandle, bashscriptname) = tempfile.mkstemp()
            bashscript = os.fdopen(bashscripthandle, 'w', newline='\n')
            bashscript.write("set -e\n")
            bashscript.write("cd %s/%s\n" % (self.tempsrc.replace('\\','/'), module))
            bashscript.write("%s gbuildtoide WORKDIR=%s SRCDIR=%s\n" % (self.make, self.tempwork.replace('\\', '/'), self.tempsrc.replace('\\','/')))
            bashscript.close()
            subprocess.check_call([self.bash, bashscriptname.replace('\\', '/')])
            os.remove(bashscriptname)
        jsonfiles = os.listdir(os.path.join(self.tempwork, 'GbuildToIde', 'Library'))
        gbuildlibs = []
        for jsonfilename in jsonfiles:
            with open(os.path.join(self.tempwork, 'GbuildToIde', 'Library', jsonfilename), 'r') as f:
                gbuildlibs.append(json.load(f))


if __name__ == "__main__":
    unittest.main()

# vim: set shiftwidth=4 softtabstop=4 expandtab:
