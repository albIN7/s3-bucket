import boto
import boto.s3.connection


access_key = 'MG17R69AX5JRF9NIE996'
secret_key = '82DoovmUtAUdQnbmn20SAdgsdeA7ZVCpYTmrL8UV'

conn = boto.connect_s3(
        aws_access_key_id = access_key,
        aws_secret_access_key = secret_key,
        host = '172.16.115.75',
        port = 8080,
        is_secure=False,               # uncomment if you are not using ssl

        calling_format = boto.s3.connection.OrdinaryCallingFormat(),
        )

#listing owned buckets
for bucket in conn.get_all_buckets():
        print "{name}\t{created}".format(
                name = bucket.name,
                created = bucket.creation_date,
        )
"""
print "creating objects"
for i in range(5):

    name = 'myobject' + str(i)
    key = bucket.new_key(name)
    key.set_contents_from_filename('mycontent')
    print name
"""
print "listing a bucket's content"
for key in bucket.list():
        print "{name}\t{size}\t{modified}".format(
                name = key.name,
                size = key.size,
                modified = key.last_modified,
                )
