

// rgw_rest_s3.h

class RGWListBucket_ObjStore_S3v2 : public RGWListBucket_ObjStore {
  bool objs_container;
public:
  RGWListBucket_ObjStore_S3v2() : objs_container(false) {
    default_max = 1000;
  }
  ~RGWListBucket_ObjStore_S3v2() override {}

  int get_params() override;
  void send_response() override;
};

//rgw_rest_s3.cc

int RGWListBucket_ObjStore_S3v2::get_params()
{
  if(s->info.args.get("ContinuationToken").empty()) return;
  list_versions = s->info.args.exists("versions");
  prefix = s->info.args.get("prefix");
  token = s->info.args.get("ContinuationToken");
  startAfter = s->info.args.get("start-after");
   // non-standard
  s->info.args.get_bool("allow-unordered", &allow_unordered, false);

  delimiter = s->info.args.get("delimiter");

  max_keys = s->info.args.get("max-keys");
  op_ret = parse_max_keys();
  if (op_ret < 0) {
    return op_ret;
  }

  encoding_type = s->info.args.get("encoding-type");
  fetchOwner = s->info.args.get("fetch-owner");
  if (s->system_request) {
    s->info.args.get_bool("objs-container", &objs_container, false);
    const char *shard_id_str = s->info.env->get("HTTP_RGWX_SHARD_ID");
    if (shard_id_str) {
      string err;
      shard_id = strict_strtol(shard_id_str, 10, &err);
      if (!err.empty()) {
        ldout(s->cct, 5) << "bad shard id specified: " << shard_id_str << dendl;
        return -EINVAL;
      }
    } else {
      shard_id = s->bucket_instance_shard_id;
    }
  }

  return 0;
}

void RGWListBucket_ObjStore_S3v2::send_response()
{
  int KeyCount;
  if (op_ret < 0)
    set_req_state_err(s, op_ret);
  dump_errno(s);

  // Explicitly use chunked transfer encoding so that we can stream the result
  // to the user without having to wait for the full length of it.
  end_header(s, this, "application/xml", CHUNKED_TRANSFER_ENCODING);
  dump_start(s);
  if (op_ret < 0)
    return;
/*
  if (list_versions) {
    send_versioned_response();
    return;
  }
  */

  s->formatter->open_object_section_in_ns("ListBucketResult", XMLNS_AWS_S3);
  if (!s->bucket_tenant.empty())
    s->formatter->dump_string("Tenant", s->bucket_tenant);
  s->formatter->dump_string("Name", s->bucket_name);
  s->formatter->dump_string("Prefix", prefix);
  s->formatter->dump_int("MaxKeys", max);
  if (!delimiter.empty())
    s->formatter->dump_string("Delimiter", delimiter);

  s->formatter->dump_string("IsTruncated", (max && is_truncated ? "true"
              : "false"));

  bool encode_key = false;
  if (strcasecmp(encoding_type.c_str(), "url") == 0) {
    s->formatter->dump_string("EncodingType", "url");
    encode_key = true;
  }

  if (op_ret >= 0) {
    vector<rgw_bucket_dir_entry>::iterator iter;
    for (iter = objs.begin(); iter != objs.end(); ++iter) {
      rgw_obj_key key(iter->key);
      s->formatter->open_array_section("Contents");
      if (encode_key) {
  string key_name;
  url_encode(key.name, key_name);
  s->formatter->dump_string("Key", key_name);
      } else {
  s->formatter->dump_string("Key", key.name);
      }
      dump_time(s, "LastModified", &iter->meta.mtime);
      s->formatter->dump_format("ETag", "\"%s\"", iter->meta.etag.c_str());
      s->formatter->dump_int("Size", iter->meta.accounted_size);
      auto& storage_class = rgw_placement_rule::get_canonical_storage_class(iter->meta.storage_class);
      s->formatter->dump_string("StorageClass", storage_class.c_str());
      dump_owner(s, iter->meta.owner, iter->meta.owner_display_name);
      if (s->system_request) {
        s->formatter->dump_string("RgwxTag", iter->tag);
      }
      s->formatter->close_section();
    }
    
    if (!common_prefixes.empty()) {
      map<string, bool>::iterator pref_iter;
      for (pref_iter = common_prefixes.begin();
     pref_iter != common_prefixes.end(); ++pref_iter) {
  s->formatter->open_array_section("CommonPrefixes");
  s->formatter->dump_string("Prefix", pref_iter->first);
  //s->formatter->dump_string("KeyCount",)   Find length of json Contents and CommonPrefixes
  s->formatter->dump_string("ContinuationToken",token);
  //Find NextContinuationToken
  s->formatter->dump_int("StartAfter", start-after);

  s->formatter->close_section();
      }
    }
  }
  s->formatter->close_section();
  rgw_flush_formatter_and_reset(s, s->formatter);
}



rgw/rgw_op.h Before RGWGetBucketLogging

class RGWListBucketv2 : public RGWOp {
protected:
  RGWBucketEnt bucket;
  string fetchOwner;
  string token;
  string StartAfter;
  string prefix;
  string max_keys;
  string delimiter;
  string encoding_type;
  bool list_versions;
  int max;
  vector<rgw_bucket_dir_entry> objs;
  map<string, bool> common_prefixes;

  int default_max;
  bool is_truncated;
  bool allow_unordered;

  int shard_id;

  int parse_max_keys();

public:
  RGWListBucket() : list_versions(false), max(0),
                    default_max(0), is_truncated(false),
		    allow_unordered(false), shard_id(-1) {}
  int verify_permission() override;
  void pre_exec() override;
  void execute() override;

  virtual int get_params() = 0;
  void send_response() override = 0;
  const char* name() const override { return "list_bucket"; }
  RGWOpType get_type() override { return RGW_OP_LIST_BUCKET; }
  uint32_t op_mask() override { return RGW_OP_TYPE_READ; }
  virtual bool need_container_stats() { return false; }
};
