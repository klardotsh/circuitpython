:mod:`gzip` -- gzip file decompression
============================================

.. module:: gzip
   :synopsis: gzip file decompression

|see_cpython_module| :mod:`cpython:gzip`.

This module enables decompression of gzip (``.gz``) files, a common format for
compressing large data (for example, CSV or TXT files).

Functions
---------

.. function:: decompress(data)

   Decompress the data, returning a bytes object containing the uncompressed data.
