# kvService

How to run a mocked version of Aws Services:

  In main.cpp change RestController constructor parameter from KvService to KmServiceMock.
  I will make it possible to switch modes through command line args.

Instructions to run locally:

1) setup your aws credentials: http://docs.aws.amazon.com/cli/latest/userguide/cli-config-files.html

2) install library dependencies:

  2.1) aws-sdk-cpp: http://docs.aws.amazon.com/sdk-for-cpp/v1/developer-guide/setup.html
  2.2) cpprestsdk: https://github.com/Microsoft/cpprestsdk
  
  (WIP)

Asap I will provide a Docker image.

------

Instructions to deploy using 
