int RGWListBucket_ObjStore_S3::get_params()
{
  list_versions = s->info.args.exists("versions");
  prefix = s->info.args.get("prefix");
/*  if (!list_versions) {
    marker = s->info.args.get("marker");
  } else {
    marker.name = s->info.args.get("key-marker");
    marker.instance = s->info.args.get("version-id-marker");
  }
  */

  // non-standard
  ContinuationToken.name = s->info.args.get("ContinuationToken");
  s->info.args.get_bool("allow-unordered", &allow_unordered, false);

  delimiter = s->info.args.get("delimiter");

  max_keys = s->info.args.get("max-keys");
  op_ret = parse_max_keys();
  if (op_ret < 0) {
    return op_ret;
  }




void RGWListBucket_ObjStore_S3::send_response()
{
  if (op_ret < 0)
    set_req_state_err(s, op_ret);
  dump_errno(s);

  // Explicitly use chunked transfer encoding so that we can stream the result
  // to the user without having to wait for the full length of it.
  end_header(s, this, "application/xml", CHUNKED_TRANSFER_ENCODING);
  dump_start(s);
  if (op_ret < 0)
    return;

  if (list_versions) {
    send_versioned_response();
    return;
  }

  s->formatter->open_object_section_in_ns("ListBucketResult", XMLNS_AWS_S3);
  if (!s->bucket_tenant.empty())
    s->formatter->dump_string("Tenant", s->bucket_tenant);
  s->formatter->dump_string("Name", s->bucket_name);
  s->formatter->dump_string("Prefix", prefix);
  //Instead of knowing the last key, we are checking the existence of nextcontinuationtoken
  s->formatter->dump_string("ContinuationToken", ContinuationToken.name);
  if (is_truncated && !NextContinuationToken.empty())
    s->formatter->dump_string(ContinuationToken.name,NextContinuationToken.name);
  s->formatter->dump_string("NextContinuationToken",NextContinuationToken.name);
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
      s->formatter->dump_string("StorageClass", "STANDARD");
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
    s->formatter->close_section();
      }
    }
  }
  s->formatter->close_section();
  rgw_flush_formatter_and_reset(s, s->formatter);
}
