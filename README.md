# kvService

=== Warning: This document is suffering constant changes!! ===

How to run a mocked version of Aws Services:

  In main.cpp change RestController constructor parameter from KvService to KmServiceMock.
  I will make it possible to switch modes through command line args.

Instructions to run locally:

1) setup your aws credentials: http://docs.aws.amazon.com/cli/latest/userguide/cli-config-files.html

2) install library dependencies:

*	aws-sdk-cpp: http://docs.aws.amazon.com/sdk-for-cpp/v1/developer-guide/setup.html

*	cpprestsdk: https://github.com/Microsoft/cpprestsdk

* protobuf: https://github.com/google/protobuf/blob/master/src/README.md
  
  (WIP)

Asap I will provide a Docker image.
