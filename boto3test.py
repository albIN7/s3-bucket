import boto3
import hashlib
import boto3.s3.connection

access_key = 'TESTER'
secret_key = 'test123'

print("Before creating connection")

conn = boto3.connect_s3(
aws_access_key_id = access_key,
aws_secret_access_key = secret_key,
port = 8000,
host = 'localhost',
is_secure=False,
validate_certs=False,
calling_format = boto3.s3.connection.OrdinaryCallingFormat(),
)

print "After creating connection"

bucket = conn.create_bucket(Bucket ='my-new-shiny-bucket')
response = conn.list_objects_v2(Bucket = 'my-new-shiny-bucket')
print response
