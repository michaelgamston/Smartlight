import json
from base64 import b64decode, b64encode
import numpy as np
import boto3

def lambda_handler(event, context):
    timestamp = event['ts']
    data = event['data']
    decoded_bytes = b64decode(data)
    data_numpy = np.frombuffer(decoded_bytes, dtype=np.uint8)
    checkbyte = b64encode(data_numpy[0]).decode('ascii')
    image = b64encode(data_numpy[1:7501]).decode('ascii')
    other_params = b64encode(data_numpy[7502:]).decode('ascii') # need to change this to reflect other info we may want to encode 
    s3 = boto3.resource(
        service_name="s3",
        region_name="eu-west-2",
        aws_access_key_id="AKIAZ252ONHLAUQMV6U2",
        aws_secret_access_key="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",
    )    
    bucket_name = "imagestestsmartlight"
    bucket = s3.Bucket(bucket_name)
    json_file = {
        'checkbyte':checkbyte,
        'image': image, 
        'other_params': other_params
    }
    bucket.put_object(Key=f"test_images/{timestamp}.json", Body=json.dumps(json_file))
    return json_file