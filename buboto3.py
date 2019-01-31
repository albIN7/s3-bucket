#import hashlib
#import boto3

#access_key= "MG17R69AX5JRF9NIE996"
#secret_key= "82DoovmUtAUdQnbmn20SAdgsdeA7ZVCpYTmrL8UV"

def listv2():
        client = boto3.client('s3',
        aws_access_key_id=get_main_aws_access_key(),
        aws_secret_access_key=get_main_aws_secret_key()
        endpoint_url='http://172.16.115.75:8080',
        region_name='',
        )
        bucket = client.create_bucket(Bucket='sts_bkt2')
        response1 = client.list_objects_v2(Bucket='sts_bkt2')
        print response1


