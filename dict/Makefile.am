include $(top_srcdir)/am.conf
dicdir		= $(libdir)/geonlp
dic_DATA	= geodic.sq3 mecabusr.dic wordlist.sq3 geo_name_fullname.drt
EXTRA_DIST	= geodic-init.dump.gz geonlp_local.rc
MAKEDIC		= ../geonlp_ma_makedic/geonlp_ma_makedic
GEONLP_ADD      = ../src/geonlp_add

geodic.sq3:	geodic-init.dump.gz
	@-rm -f $(dic_DATA);
	@echo "データベースファイルを初期化しています。"
	@gzip -dc geodic-init.dump.gz | sqlite3 $@
	if test -f geonlp-data/*_u.zip; \
	then \
	@echo "辞書 zip ファイルを展開しています。"; \
	@cd geonlp-data; for zip in *_u.zip; do unzip -o $${zip} > /dev/null; done; \
	fi
	@echo "辞書 CSV ファイルをインポートしています。"
	@for csv in geonlp-data/*/*.csv; do \
          echo $${csv%.*}; \
	  $(GEONLP_ADD) --rc=./geonlp_local.rc $${csv%.*}.json $${csv}; \
        done
	@echo "データベースファイルを作成しました。"

mecabusr.dic:	geodic.sq3 $(MAKEDIC) geonlp_local.rc
	$(MAKEDIC) -u -f ../etc/geonlp_ma_makedic.rc ./geonlp_local.rc

wordlist.sq3:	geodic.sq3 $(MAKEDIC) geonlp_local.rc
	$(MAKEDIC) -u -f ../etc/geonlp_ma_makedic.rc ./geonlp_local.rc

geo_name_fullname.drt:	geodic.sq3 $(MAKEDIC) geonlp_local.rc
	-rm -f $@
	$(MAKEDIC) -u -f ../etc/geonlp_ma_makedic.rc ./geonlp_local.rc

clean:
	-rm -f $(dic_DATA)

test-api-local:
	cat ../test/geonlp_api_test.json | ../src/geonlp_api --rc=geonlp_local.rc
