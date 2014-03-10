#!ruby
# -*- coding: utf-8 -*-
require "../GeonlpMA"

msg = '東京大学は文京区の他、目黒区や千葉県柏市にもあり ます。'
gs = GeonlpMA.new("/usr/local/etc/geonlp_ma.rc")

result = gs.parseNode(msg)
result.each do |node|
  printf("%s\t%s,%s,%s,%s,%s,%s,%s,%s,%s\n", \
	 node['surface'], node['partOfSpeech'], \
	 node['subclassification1'], node['subclassification2'], node['subclassification3'], \
	 node['conjugatedForm'], node['conjugationType'], \
	 node['originalForm'], node['yomi'], node['pronunciation'])
  if node['subclassification2'] == "地名語"
    geoword_entries = node['subclassification3'].split("/")
    geoword_entries.each do |geoword_entry|
      if /([^\.]+)\.(\d+):(.*)/ =~ geoword_entry
        gw = gs.getGeowordEntry($2.to_i) # $1 にはテーマID，$3 には語尾が入る
      end
      printf(" | %d\t%s,%s,%s,%s,%s,%s,%s,%s,%f,%f,%s,%s,%s,%s,%s,%s\n", \
	     gw['id'], gw['geoword'], gw['upper_geowords'].join("|"), \
	     gw['desinence'], gw['yomi'], gw['pron'], \
	     gw['geoword_full'], gw['yomi_full'], gw['pron_full'], \
	     gw['latitude'], gw['longitude'], \
	     gw['theme_id'], gw['code'], gw['state_code'], gw['city_code'], \
	     gw['address'], gw['note'])
    end
  end
end

