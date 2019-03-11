import boto3
import json
import logging

logging.basicConfig(filename="boto.log", level=logging.DEBUG)
access_key = 'TESTER1'
secret_key = 'test123'
s3client = boto3.client('s3',
aws_access_key_id = '0555b35654ad1656d804',
#aws_access_key_id = 'TESTER1',
aws_secret_access_key = 'h7GhxuBLTrlhVUyxSPUKUV8r/2EI4ngqJxD7iBdBYLhwluN30JaT3Q==' ,
#aws_secret_access_key = 'test123',
endpoint_url='http://s3.us-east.localhost:8000',
#endpoint_url='http://10.8.128.43:8080',
region_name='',)
#print "After creating S3 connection"
bucket_name = 'my-bucket'
s3bucket = s3client.create_bucket(Bucket=bucket_name)
for j in range(5):
	s3client.put_object(Bucket=bucket_name, Key=str(j))
response = s3client.list_objects_v2(Bucket=bucket_name)
print(response)
