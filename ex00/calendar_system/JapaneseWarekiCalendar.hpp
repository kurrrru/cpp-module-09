#pragma once

#include <string>

#include <calendar_system/ICalendarSystem.hpp>

namespace toolbox {

class JapaneseWarekiCalendar : public ICalendarSystem {
 public:
    JapaneseWarekiCalendar();
    JapaneseWarekiCalendar(const JapaneseWarekiCalendar& other);
    JapaneseWarekiCalendar& operator=(const JapaneseWarekiCalendar& other);
    ~JapaneseWarekiCalendar();

    int to_serial_date(int era, int year, int month, int day) const;
    int to_serial_date(const std::string& date_str,
        const char* format, bool strict) const;
    void from_serial_date(int serial_date,
        int& era, int& year, int& month, int& day) const;
    void from_serial_date(int serial_date,
        std::string& date_str, const char* format) const;
    void from_serial_date(int serial_date,
        int& day_of_week) const;  // 0=Sun, 1=Mon, ..., 6=Sat

    enum Era {
        // Asuka period
        TAIKA,  // 645-650
        HAKUCHI,  // 650-655
        SHUCHO,  // 686-687
        TAIHO,  // 701-704
        KEIUN,  // 704-708
        WADO,  // 708-715

        // Nara period
        REIKI,  // 715-717
        YORO,  // 717-724
        JINKI,  // 724-729
        TEMPYO,  // 729-749
        TEMPYO_KAMPO,  // 749-749
        TEMPYO_SHOHO,  // 749-757
        TEMPYO_HOJI,  // 757-765
        TEMPYO_JINGO,  // 765-767
        JINGO_KEIUN,  // 767-770
        HOKI,  // 770-781
        TEN_O,  // 781-782 (天応) ※TENOUだと天応/天王の区別がつかないため
        ENRYAKU,  // 782-806

        // Heian period
        DAIDO,  // 806-810
        KONIN,  // 810-824
        TENCHO,  // 824-834
        JOWA,  // 834-848
        KAJO,  // 848-851
        NINJU,  // 851-854
        SAIKO,  // 854-857
        TEN_AN,  // 857-859
        JOGAN,  // 859-877
        GANGYO,  // 877-885
        NINNA,  // 885-889
        KAMPYO,  // 889-898
        SHOTAI,  // 898-901
        ENGI,  // 901-923
        ENCHO,  // 923-931
        JOHEI,  // 931-938
        TENKYO,  // 938-947
        TENRYAKU,  // 947-957
        TENTOKU,  // 957-961
        O_WA,  // 961-964 (応和)
        KO_HO,  // 964-968 (康保)
        ANNA,  // 968-970
        TENROKU,  // 970-973
        TEN_EN,  // 973-976 (天延)
        JOGEN,  // 976-978 (貞元)
        TENGEN,  // 978-983
        EIKAN,  // 983-985
        KANNA,  // 985-987
        EIEN,  // 987-989
        EISO,  // 989-990
        SHORYAKU,  // 990-995
        CHOTOKU,  // 995-999
        CHO_HO,  // 999-1004 (長保)
        KANKO,  // 1004-1012
        CHOWA,  // 1012-1017
        KAN_NIN,  // 1017-1021 (寛仁)
        JIAN,  // 1021-1024
        MANJU,  // 1024-1028
        CHOGEN,  // 1028-1037 (長元)
        CHORYAKU,  // 1037-1040
        CHOKYU,  // 1040-1044
        KANTOKU,  // 1044-1046
        EISHO,  // 1046-1053 (永承)
        TENGI,  // 1053-1058
        KOHEI,  // 1058-1065
        JIRYAKU,  // 1065-1069
        ENKYU,  // 1069-1074
        JO_HO,  // 1074-1077 (承保)
        JORYAKU,  // 1077-1081
        EI_HO,  // 1081-1084 (永保)
        O_TOKU,  // 1084-1087 (応徳)
        KANJI,  // 1087-1094
        KA_HO,  // 1094-1096 (嘉保)
        EICHO,  // 1096-1097
        JOTOKU,  // 1097-1099 (承徳)
        KOWA,  // 1099-1104
        CHOJI,  // 1104-1106
        KASHO,  // 1106-1108
        TEN_NIN,  // 1108-1110 (天仁)
        TEN_EI,  // 1110-1113 (天永)
        EIKYU,  // 1113-1118
        GEN_EI,  // 1118-1120
        HO_AN,  // 1120-1124
        TENJI,  // 1124-1126
        DAIJI,  // 1126-1131
        TENSHO,  // 1131-1132 (天承)
        CHOSHO,  // 1132-1135 (長承)
        HO_EN,  // 1135-1141 (保延)
        EIJI,  // 1141-1142
        KOJI,  // 1142-1144
        TEN_YO,  // 1144-1145
        KYU_AN,  // 1145-1151
        NIMPEI,  // 1151-1154
        KYUJU,  // 1154-1156
        HOGEN,  // 1156-1159
        HEIJI,  // 1159-1160
        EIRYAKU,  // 1160-1161
        O_HO,  // 1161-1163 (応保)
        CHOKAN,  // 1163-1165
        EIMAN,  // 1165-1166
        NIN_AN,  // 1166-1169
        KA_O,  // 1169-1171
        JO_AN,  // 1171-1175
        ANGEN,  // 1175-1177
        JISHO,  // 1177-1181
        YO_WA,  // 1181-1182 (養和)
        JUEI,  // 1182-1184
        GENRYAKU,  // 1184-1185

        // Kamakura period
        BUNJI,  // 1185-1190
        KENKYU,  // 1190-1199
        SHOJI,  // 1199-1201
        KENNIN,  // 1201-1204
        GENKYU,  // 1204-1206
        KEN_EI,  // 1206-1207
        JOGEN_K,  // 1207-1211 (承元) ※Jogen重複のため
        KENRYAKU,  // 1211-1213
        KEMPO,  // 1213-1219
        JOKYU,  // 1219-1222
        JO_O,  // 1222-1224 (貞応)
        GENNIN,  // 1224-1225
        KAROKU,  // 1225-1227
        ANTEI,  // 1227-1229 (安貞)
        KANGI,  // 1229-1232
        JOEI,  // 1232-1233
        TENPUKU,  // 1233-1234
        BUNRYAKU,  // 1234-1235
        KATEI,  // 1235-1238
        RYAKUNIN,  // 1238-1239
        EN_O,  // 1239-1240
        NINJI,  // 1240-1243
        KANGEN,  // 1243-1247
        HOJI,  // 1247-1249
        KENCHO,  // 1249-1256
        KOGEN,  // 1256-1257
        SHOKA,  // 1257-1259
        SHOGEN,  // 1259-1260
        BUN_O,  // 1260-1261
        KOCHO,  // 1261-1264
        BUN_EI,  // 1264-1275
        KENJI,  // 1275-1278
        KO_AN,  // 1278-1288
        SHO_O,  // 1288-1293
        EININ,  // 1293-1299
        SHO_AN_K,  // 1299-1302 (正安) ※Sho_an重複のため
        KENGEN,  // 1302-1303
        KAGEN,  // 1303-1306
        TOKUJI,  // 1306-1308
        ENKYO_K,  // 1308-1311 (延慶) ※Enkyo重複のため
        O_CHO,  // 1311-1312
        SHOWA_K,  // 1312-1317 (正和) ※Showa重複のため
        BUMPO,  // 1317-1319
        GEN_O,  // 1319-1321
        GENKYO,  // 1321-1324
        SHOCHU,  // 1324-1326
        KARYAKU,  // 1326-1329
        GENTOKU,  // 1329-1331

        // Daikakuji line
        GENKO,  // 1331-1334

        // Jimyoin line
        SHOKYO,  // 1332-1333

        // Kenmu Restoration
        KENMU,  // 1334-1336 (Southern Court until 1338)

        // Nanboku-cho period (Northern and Southern Courts)

        // Southern Court
        ENGEN,  // 1336-1340
        KOKOKU,  // 1340-1346
        SHOHEI,  // 1346-1370
        KENTOKU,  // 1370-1372
        BUNCHU,  // 1372-1375
        TENJU,  // 1375-1381
        KOWA_S,  // 1381-1384 (弘和) ※Kowa重複のため
        GENCHU,  // 1384-1392

        // Northern Court
        RYAKUO,  // 1338-1342
        KOEI,  // 1342-1345
        JOWA_N,  // 1345-1350 (貞和) ※Jowa重複のため
        KAN_O,  // 1350-1352
        BUNNA,  // 1352-1356
        EMBUN,  // 1356-1361
        KO_AN_N,  // 1361-1362 (康安) ※Ko_an重複のため
        JOJI_N,  // 1362-1368 (貞治)
        O_AN,  // 1368-1375
        EIWA,  // 1375-1379
        KORYAKU,  // 1379-1381
        EITOKU,  // 1381-1384
        SHITOKU,  // 1384-1387
        KAKYO,  // 1387-1389
        KO_O,  // 1389-1390
        MEITOKU,  // 1390-1394 (1392年に南北朝合一)

        // Muromachi period
        OEI,  // 1394-1428
        SHOCHŌ,  // 1428-1429
        EIKYO,  // 1429-1441
        KAKITSU,  // 1441-1444
        BUN_AN,  // 1444-1449
        HOTOKU,  // 1449-1452
        KYOTOKU,  // 1452-1455
        KOSHO,  // 1455-1457
        CHOROKU,  // 1457-1460
        KANSHO,  // 1460-1466
        BUNSHO,  // 1466-1467
        ONIN,  // 1467-1469
        BUNMEI,  // 1469-1487
        CHOKYO,  // 1487-1489
        ENTOKU,  // 1489-1492
        MEI_O,  // 1492-1501
        BUNKI,  // 1501-1504
        EISHŌ_M,  // 1504-1521 (永正) ※Eisho重複のため
        TAIEI,  // 1521-1528
        KYOROKU,  // 1528-1532
        TEMBUN,  // 1532-1555
        KOJI_M,  // 1555-1558 (弘治) ※Koji重複のため
        EIROKU,  // 1558-1570
        GENKI,  // 1570-1573

        // Azuchi-Momoyama period
        TENSHO_A,  // 1573-1592 (天正) ※Tensho重複のため
        BUNROKU,  // 1592-1596
        KEICHO,  // 1596-1615

        // Edo period
        GENNA,  // 1615-1624
        KAN_EI,  // 1624-1644
        SHO_HO,  // 1644-1648
        KEIAN,  // 1648-1652
        JO_O_E,  // 1652-1655 (承応) ※Jo_o重複のため
        MEIREKI,  // 1655-1658
        MANJI,  // 1658-1661
        KANBUN,  // 1661-1673
        EMPO,  // 1673-1681
        TENNA,  // 1681-1684
        JOKYO,  // 1684-1688
        GENROKU,  // 1688-1704
        HOEI,  // 1704-1711
        SHOTOKU,  // 1711-1716
        KYO_HO,  // 1716-1736
        GEMBUN,  // 1736-1741
        KAMPO_E,  // 1741-1744 (寛保) ※Kampo重複のため
        ENKYO_E,  // 1744-1748 (延享) ※Enkyo重複のため
        KAN_EN,  // 1748-1751
        HOREKI,  // 1751-1764
        MEIWA,  // 1764-1772
        AN_EI,  // 1772-1781
        TENMEI,  // 1781-1789
        KANSEI,  // 1789-1801
        KYO_WA,  // 1801-1804
        BUNKA,  // 1804-1818
        BUNSEI,  // 1818-1830
        TEMPO,  // 1830-1844
        KOKA,  // 1844-1848
        KAEI,  // 1848-1854
        ANSEI,  // 1854-1860
        MAN_EN,  // 1860-1861
        BUNKYU,  // 1861-1864
        GENJI,  // 1864-1865
        KEI_O,  // 1865-1868

        // Modern and Contemporary
        MEIJI,  // 1868-1912
        TAISHO,  // 1912-1926
        SHOWA,  // 1926-1989
        HEISEI,  // 1989-2019
        REIWA,  // 2019-

        END_OF_ERA  // 終焉
    };
};

}  // namespace toolbox
