# AWS Lambda how to

Data comes in from MQTT as base64 encoded binary (event['data'] JSON key). This needs decoding with e.g. base64 Python library. The image will need re-encoding in base64 in order to JSONify. 

## Dependencies
Follow instructions [here](https://docs.aws.amazon.com/lambda/latest/dg/python-package.html) to build project with dependencies.

## ToDos 

Need to allow permissions for Lambda to receive payload from MQTT and then also to write to S3.
