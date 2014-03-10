include $(top_srcdir)/am.conf
dicdir		= $(libdir)/geonlp
dic_DATA	= geodic.sq3 mecabusr.dic wordlist.sq3 geo_name_fullname.drt
EXTRA_DIST	= geodic-init.dump.gz geonlp_local.rc zip/*_u.zip
MAKEDIC		= ../geonlp_ma_makedic/geonlp_ma_makedic
GEONLP_ADD      = ../src/geonlp_add

geodic.sq3:	geodic-init.dump.gz zip/*_u.zip
	@-rm -f $@;
	@echo "データベースファイルを初期化しています。"
	@gzip -dc geodic-init.dump.gz | sqlite3 $@
	@echo "辞書 zip ファイルをインポートしています。"
	@cd zip; for zip in *_u.zip; do unzip -o $${zip} > /dev/null; done
	@for json in zip/*/*.json; do \
          echo $${json%.*}; \
	  $(GEONLP_ADD) --rc=./geonlp_local.rc $${json} $${json%.*}.csv; \
        done
	@rm zip/*/*.csv zip/*/*.json zip/*/*.license.txt
	@echo "データベースファイルを作成しました。"

mecabusr.dic:	geodic.sq3 $(MAKEDIC) geonlp_local.rc
	$(MAKEDIC) -u -f ../etc/geonlp_ma_makedic.rc ./geonlp_local.rc

wordlist.sq3:	geodic.sq3 $(MAKEDIC) geonlp_local.rc
	$(MAKEDIC) -u -f ../etc/geonlp_ma_makedic.rc ./geonlp_local.rc

geo_name_fullname.drt:	geodic.sq3 $(MAKEDIC) geonlp_local.rc
	-rm -f $@
	$(MAKEDIC) -u -f ../etc/geonlp_ma_makedic.rc ./geonlp_local.rc

clean:
	-rm -f $(dic_DATA) zip/*/*.csv zip/*/*.json zip/*/*.license.txt

test-api-local:
	cat ../test/geonlp_api_test.json | ../src/geonlp_api --rc=geonlp_local.rc
