# s3-bucket

Go through the following documentation

http://boto3.readthedocs.io/en/latest/guide/migrations3.html

Creating the Connection

Boto 3 has both low-level clients and higher-level resources. For Amazon S3, the higher-level resources are the most similar to Boto 2.x's s3 module:

Boto 2.x import boto

s3_connection = boto.connect_s3()
Boto 3

import boto3

s3 = boto3.resource('s3')
Creating a Bucket

Creating a bucket in Boto 2 and Boto 3 is very similar, except that in Boto 3 all action parameters must be passed via keyword arguments and a bucket configuration must be specified manually:

Boto 2.x

s3_connection.create_bucket('mybucket')

s3_connection.create_bucket('mybucket', location=Location.USWest)
Boto 3

s3.create_bucket(Bucket='mybucket')

s3.create_bucket(Bucket='mybucket', CreateBucketConfiguration={
    'LocationConstraint': 'us-west-1'})
Storing Data

Storing data from a file, stream, or string is easy:

Boto 2.x

from boto.s3.key import Key

key = Key('hello.txt')

key.set_contents_from_file('/tmp/hello.txt')
Boto 3

s3.Object('mybucket', 'hello.txt').put(Body=open('/tmp/hello.txt', 'rb'))
