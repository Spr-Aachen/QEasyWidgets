# -*- coding:utf-8 -*-

from setuptools import setup, find_packages

##############################################################################################################################

with open('./README.md', encoding = 'utf-8') as f:
    LongDescription = f.read()

##############################################################################################################################

setup(
    name = "QEasyWidgets",
    version = '0.6.7',
    description = 'A simple widget library based on PySide6',
    long_description = LongDescription,
    long_description_content_type = 'text/markdown',
    license = 'GPLv3',
    author = "Spr_Aachen",
    author_email = '2835946988@qq.com',
    url = 'https://github.com/Spr-Aachen/QEasyWidgets',
    project_urls = {
        'Source Code': 'https://github.com/Spr-Aachen/QEasyWidgets',
        'Bug Tracker': 'https://github.com/Spr-Aachen/QEasyWidgets/issues',
    },
    packages = find_packages(
        where = '.',
        exclude = ()
    ),
    include_package_data = True,
    install_requires = [
        "PySide6",
        "pywin32;platform_system=='Windows'",
        "darkdetect",
        "PyEasyUtils"
    ],
    classifiers = [
        'Operating System :: OS Independent',
        'Programming Language :: Python :: 3',
        'License :: OSI Approved :: GNU General Public License v3 (GPLv3)',
    ]
)

##############################################################################################################################