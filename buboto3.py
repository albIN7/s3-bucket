import hashlib
import boto3

access_key= "MG17R69AX5JRF9NIE996"
secret_key= "82DoovmUtAUdQnbmn20SAdgsdeA7ZVCpYTmrL8UV"

client = boto3.client('s3',
aws_access_key_id=access_key,
aws_secret_access_key=secret_key,
endpoint_url='http://172.16.115.75:8080',
region_name='',
)


bucket = client.create_bucket(Bucket='sts_bkt2')
bucket = client.create_bucket(Bucket='sts_bkt3')
response = client.list_buckets()
for bucket in response["Buckets"]:
        print "{name}\t{created}".format(
                    name = bucket['Name'],
                    created = bucket['CreationDate'],
        )
