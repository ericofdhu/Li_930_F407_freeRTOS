//"********************Æ´ÒôÊäÈë·¨Ä£¿é********************"

//"******************************************************************************"
#include "MyPinYin.h"
#include <stdint.h>
#include <string.h>



//"Æ´ÒôÊäÈë·¨ºº×ÖÅÅÁĞ±í"
const char PY_mb_a     []={"°¢°¡"};
const char PY_mb_ai    []={"°¥°§°¦°£°¤°¨°©°«°ª°¬°®°¯°­"};
const char PY_mb_an    []={"°²°±°°°³°¶°´°¸°·°µ"};
const char PY_mb_ang   []={"°¹°º°»"};
const char PY_mb_ao    []={"°¼°½°¾°¿°À°Á°Â°Ä°Ã"};
const char PY_mb_ba    []={"°Ë°Í°È°Ç°É°Å°Ì°Æ°Ê°Î°Ï°Ñ°Ğ°Ó°Ö°Õ°Ô"};
const char PY_mb_bai   []={"°×°Ù°Û°Ø°Ú°Ü°İ°Ş"};
const char PY_mb_ban   []={"°â°à°ã°ä°ß°á°å°æ°ì°ë°é°ç°è°í°ê"};
const char PY_mb_bang  []={"°î°ï°ğ°ó°ñ°ò°ö°ø°ô°ù°õ°÷"};
const char PY_mb_bao   []={"°ü°ú°û°ı±¢±¦±¥±£±¤±¨±§±ª±«±©±¬°ş±¡ÆÙ"};
const char PY_mb_bei   []={"±°±­±¯±®±±±´±·±¸±³±µ±¶±»±¹±º±²"};
const char PY_mb_ben   []={"±¼±¾±½±¿º»"};
const char PY_mb_beng  []={"±À±Á±Â±Ã±Å±Ä"};
const char PY_mb_bi    []={"±Æ±Ç±È±Ë±Ê±É±Ò±Ø±Ï±Õ±Ó±Ñ±İ±Ğ±Ö±Ô±Í±×±Ì±Î±Ú±Ü±Û"};
const char PY_mb_bian  []={"±ß±à±Ş±á±â±å±ã±ä±é±æ±ç±è"};
const char PY_mb_biao  []={"±ë±ê±ì±í"};
const char PY_mb_bie   []={"±ï±î±ğ±ñ"};
const char PY_mb_bin   []={"±ö±ò±ó±õ±ô±÷"};
const char PY_mb_bing  []={"±ù±ø±û±ü±ú±ş±ı²¢²¡"};
const char PY_mb_bo    []={"²¦²¨²£²§²±²¤²¥²®²µ²¯²´²ª²¬²°²©²³²«²­²²²·"};
const char PY_mb_bu    []={"²¹²¸²¶²»²¼²½²À²¿²º²¾"};
const char PY_mb_ca    []={"²Á"};
const char PY_mb_cai   []={"²Â²Å²Ä²Æ²Ã²É²Ê²Ç²È²Ë²Ì"};
const char PY_mb_can   []={"²Î²Í²Ğ²Ï²Ñ²Ò²Ó"};
const char PY_mb_cang  []={"²Ö²×²Ô²Õ²Ø"};
const char PY_mb_cao   []={"²Ù²Ú²Ü²Û²İ"};
const char PY_mb_ce    []={"²á²à²Ş²â²ß"};
const char PY_mb_ceng  []={"²ã²äÔø"};
const char PY_mb_cha   []={"²æ²å²é²ç²è²ë²ì²ê²í²ï²îÉ²"};
const char PY_mb_chai  []={"²ğ²ñ²ò"};
const char PY_mb_chan  []={"²ô²ó²÷²ö²ø²õ²ú²ù²û²ü"};
const char PY_mb_chang []={"²ı²ş³¦³¢³¥³£³§³¡³¨³©³«³ª"};
const char PY_mb_chao  []={"³­³®³¬³²³¯³°³±³³³´´Â"};
const char PY_mb_che   []={"³µ³¶³¹³¸³·³º"};
const char PY_mb_chen  []={"³»³¾³¼³À³Á³½³Â³¿³Ä³Ã"};
const char PY_mb_cheng []={"³Æ³Å³É³Ê³Ğ³Ï³Ç³Ë³Í³Ì³Î³È³Ñ³Ò³Ó"};
const char PY_mb_chi   []={"³Ô³Õ³Ú³Ø³Û³Ù³Ö³ß³Ş³İ³Ü³â³à³ã³á"};
const char PY_mb_chong []={"³ä³å³æ³ç³è"};
const char PY_mb_chou  []={"³é³ğ³ñ³ë³î³í³ï³ê³ì³ó³ò³ô"};
const char PY_mb_chu   []={"³ö³õ³ı³ø³ü³ú³û³÷³ù´¡´¢³ş´¦´¤´¥´£Ğó"};
const char PY_mb_chuai []={"´§"};
const char PY_mb_chuan []={"´¨´©´«´¬´ª´­´®"};
const char PY_mb_chuang[]={"´³´¯´°´²´´"};
const char PY_mb_chui  []={"´µ´¶´¹´·´¸"};
const char PY_mb_chun  []={"´º´»´¿´½´¾´¼´À"};
const char PY_mb_chuo  []={"´Á"};
const char PY_mb_ci    []={"´Ã´Ê´Ä´É´È´Ç´Å´Æ´Ë´Î´Ì´Í"};
const char PY_mb_cong  []={"´Ñ´Ó´Ò´Ğ´Ï´Ô"};
const char PY_mb_cou   []={"´Õ"};
const char PY_mb_cu    []={"´Ö´Ù´×´Ø"};
const char PY_mb_cuan  []={"´Ú´Ü´Û"};
const char PY_mb_cui   []={"´Ş´ß´İ´à´ã´á´â´ä"};
const char PY_mb_cun   []={"´å´æ´ç"};
const char PY_mb_cuo   []={"´ê´è´é´ì´ë´í"};
const char PY_mb_da    []={"´î´ï´ğ´ñ´ò´ó"};
const char PY_mb_dai   []={"´ô´õ´ö´ú´ø´ıµ¡´ù´û´ü´ş´÷"};
const char PY_mb_dan   []={"µ¤µ¥µ£µ¢µ¦µ¨µ§µ©µ«µ®µ¯µ¬µ­µ°µª"};
const char PY_mb_dang  []={"µ±µ²µ³µ´µµ"};
const char PY_mb_dao   []={"ë®µ¶µ¼µºµ¹µ·µ»µ¸µ½µ¿µÁµÀµ¾"};
const char PY_mb_de    []={"µÃµÂµÄ"};
const char PY_mb_deng  []={"µÆµÇµÅµÈµËµÊµÉ"};
const char PY_mb_di    []={"µÍµÌµÎµÒµÏµĞµÓµÑµÕµ×µÖµØµÜµÛµİµÚµŞµÙ"};
const char PY_mb_dian  []={"µàµáµßµäµãµâµçµèµéµêµæµëµíµìµîµå"};
const char PY_mb_diao  []={"µóµğµòµïµñµõµöµô"};
const char PY_mb_die   []={"µùµøµüµıµşµúµû"};
const char PY_mb_ding  []={"¶¡¶£¶¢¶¤¶¥¶¦¶©¶¨¶§"};
const char PY_mb_diu   []={"¶ª"};
const char PY_mb_dong  []={"¶«¶¬¶­¶®¶¯¶³¶±¶²¶°¶´"};
const char PY_mb_dou   []={"¶¼¶µ¶·¶¶¶¸¶¹¶º¶»"};
const char PY_mb_du    []={"¶½¶¾¶Á¶¿¶À¶Â¶Ä¶Ã¶Ê¶Å¶Ç¶È¶É¶Æ"};
const char PY_mb_duan  []={"¶Ë¶Ì¶Î¶Ï¶Ğ¶Í"};
const char PY_mb_dui   []={"¶Ñ¶Ó¶Ô¶Ò"};
const char PY_mb_dun   []={"¶Ö¶Ø¶Õ¶×¶Ü¶Û¶Ù¶İ"};
const char PY_mb_duo   []={"¶à¶ß¶á¶Ş¶ä¶â¶ã¶ç¶é¶æ¶è¶å"};
const char PY_mb_e     []={"¶ï¶í¶ğ¶ë¶ì¶ê¶î¶ò¶ó¶ñ¶ö¶õ¶ô"};
const char PY_mb_en    []={"¶÷"};
const char PY_mb_er    []={"¶ù¶ø¶û¶ú¶ı¶ü¶ş·¡"};
const char PY_mb_fa    []={"·¢·¦·¥·£·§·¤·¨·©"};
const char PY_mb_fan   []={"·«·¬·­·ª·²·¯·°·³·®·±·´·µ·¸·º·¹·¶··"};
const char PY_mb_fang  []={"·½·»·¼·À·Á·¿·¾·Â·Ã·Ä·Å"};
const char PY_mb_fei   []={"·É·Ç·È·Æ·Ê·Ë·Ì·Í·Ï·Ğ·Î·Ñ"};
const char PY_mb_fen   []={"·Ö·Ô·×·Ò·Õ·Ó·Ø·Ú·Ù·Û·İ·Ü·Ş·ß·à"};
const char PY_mb_feng  []={"·á·ç·ã·â·è·å·é·æ·ä·ë·ê·ì·í·ï·î"};
const char PY_mb_fo    []={"·ğ"};
const char PY_mb_fou   []={"ó¾·ñ"};
const char PY_mb_fu    []={"·ò·ô·õ·ó¸¥·ü·ö·÷·ş·ı·ú¸¡¸¢·û¸¤·ù¸£·ø¸§¸¦¸®¸«¸©¸ª¸¨¸­¸¯¸¸¸¼¸¶¸¾¸º¸½¸À¸·¸´¸°¸±¸µ¸»¸³¸¿¸¹¸²"};
const char PY_mb_ga    []={"¸Â¸Á"};
const char PY_mb_gai   []={"¸Ã¸Ä¸Æ¸Ç¸È¸Å"};
const char PY_mb_gan   []={"¸É¸Ê¸Ë¸Î¸Ì¸Í¸Ñ¸Ï¸Ò¸Ğ¸Ó"};
const char PY_mb_gang  []={"¸Ô¸Õ¸Ú¸Ù¸Ø¸×¸Ö¸Û¸Ü"};
const char PY_mb_gao   []={"¸Ş¸á¸ß¸à¸İ¸â¸ã¸å¸ä¸æ"};
const char PY_mb_ge    []={"¸ê¸í¸ç¸ì¸ë¸î¸é¸è¸ó¸ï¸ñ¸ğ¸ô¸ö¸÷¸õ¿©"};
const char PY_mb_gei   []={"¸ø"};
const char PY_mb_gen   []={"¸ù¸ú"};
const char PY_mb_geng  []={"¸ü¸ı¸û¸ş¹¡¹¢¹£"};
const char PY_mb_gong  []={"¹¤¹­¹«¹¦¹¥¹©¹¬¹§¹ª¹¨¹®¹¯¹°¹²¹±"};
const char PY_mb_gou   []={"¹´¹µ¹³¹·¹¶¹¹¹º¹¸¹»"};
const char PY_mb_gu    []={"¹À¹¾¹Ã¹Â¹Á¹½¹¼¹¿¹Å¹È¹É¹Ç¹Æ¹Ä¹Ì¹Ê¹Ë¹Í"};
const char PY_mb_gua   []={"¹Ï¹Î¹Ğ¹Ñ¹Ò¹Ó"};
const char PY_mb_guai  []={"¹Ô¹Õ¹Ö"};
const char PY_mb_guan  []={"¹Ø¹Û¹Ù¹Ú¹×¹İ¹Ü¹á¹ß¹à¹Ş"};
const char PY_mb_guang []={"¹â¹ã¹ä"};
const char PY_mb_gui   []={"¹é¹ç¹ê¹æ¹ë¹è¹å¹ì¹î¹ï¹í¹ô¹ñ¹ó¹ğ¹ò"};
const char PY_mb_gun   []={"¹õ¹ö¹÷"};
const char PY_mb_guo   []={"¹ù¹ø¹ú¹û¹ü¹ı"};
const char PY_mb_ha    []={"¸ò¹ş"};
const char PY_mb_hai   []={"º¢º¡º£º¥º§º¦º¤»¹àË"};
const char PY_mb_han   []={"º¨º©º¬ºªº¯º­º®º«º±º°ººº¹ºµº·º´º¸º¶º³º²"};
const char PY_mb_hang  []={"º¼º½ĞĞ"};
const char PY_mb_hao   []={"ºÁºÀº¿º¾ºÃºÂºÅºÆºÄ"};
const char PY_mb_he    []={"ºÇºÈºÌºÏºÎºÍºÓºÒºËºÉºÔºĞºÊºØºÖºÕº×"};
const char PY_mb_hei   []={"ºÚºÙ"};
const char PY_mb_hen   []={"ºÛºÜºİºŞ"};
const char PY_mb_heng  []={"ºàºßºãºáºâ"};
const char PY_mb_hong  []={"ºäºåºæºëºìºêºéºçºè"};
const char PY_mb_hou   []={"ºîºíºïºğºóºñºò"};
const char PY_mb_hu    []={"ºõºôºö»¡ºüºúºøºşºùº÷ºıºû»¢»£»¥»§»¤»¦"};
const char PY_mb_hua   []={"»¨»ª»©»¬»«»¯»®»­»°"};
const char PY_mb_huai  []={"»³»²»´»±»µ"};
const char PY_mb_huan  []={"»¶»¹»·»¸»º»Ã»Â»½»»»Á»¼»À»¾»¿"};
const char PY_mb_huang []={"»Ä»Å»Ê»Ë»Æ»Ì»Í»È»Ç»É»Ğ»Î»Ñ»Ï"};
const char PY_mb_hui   []={"»Ò»Ö»Ó»Ô»Õ»Ø»×»Ú»Ü»ã»á»ä»æ»å»â»ß»Ş»à»İ»Ù»Û"};
const char PY_mb_hun   []={"»è»ç»é»ë»ê»ì"};
const char PY_mb_huo   []={"îØ»í»î»ğ»ï»ò»õ»ñ»ö»ó»ô"};
const char PY_mb_ji    []={"¼¥»÷¼¢»ø»ú¼¡¼¦¼£¼§»ı»ù¼¨¼©»û»ş»ü¼¤¼°¼ª¼³¼¶¼´¼«¼±¼²¼¬¼¯¼µ¼­¼®¼¸¼º¼·¼¹¼Æ¼Ç¼¿¼Í¼Ë¼É¼¼¼Ê¼Á¼¾¼È¼Ã¼Ì¼Å¼Ä¼Â¼À¼»¼½½å"};
const char PY_mb_jia   []={"¼Ó¼Ğ¼Ñ¼Ï¼Ò¼Î¼Ô¼Õ¼×¼Ö¼Ø¼Û¼İ¼Ü¼Ù¼Ş¼ÚĞ®"};
const char PY_mb_jian  []={"¼é¼â¼á¼ß¼ä¼ç¼è¼æ¼à¼ã¼ê¼å¼ğ¼ó¼í¼ë¼ñ¼õ¼ô¼ì¼ï¼ò¼î¼û¼ş½¨½¤½£¼ö¼ú½¡½§½¢½¥½¦¼ù¼ø¼ü¼ı"};
const char PY_mb_jiang []={"½­½ª½«½¬½©½®½²½±½°½¯½³½µ½´"};
const char PY_mb_jiao  []={"½»½¼½¿½½½¾½º½·½¹½¶½¸½Ç½Æ½Ê½È½Ã½Å½Â½Á½Ë½É½Ğ½Î½Ï½Ì½Ñ½Í¾õ½À"};
const char PY_mb_jie   []={"½×½Ô½Ó½Õ½Ò½Ö½Ú½Ù½Ü½à½á½İ½Ş½Ø½ß½ã½â½é½ä½æ½ì½ç½ê½ë½è"};
const char PY_mb_jin   []={"½í½ñ½ï½ğ½ò½î½ó½ö½ô½÷½õ¾¡¾¢½ü½ø½ú½ş½ı½û½ù"};
const char PY_mb_jing  []={"¾©¾­¾¥¾£¾ª¾§¾¦¾¬¾¤¾«¾¨¾®¾±¾°¾¯¾»¾¶¾·¾º¾¹¾´¾¸¾³¾²¾µ"};
const char PY_mb_jiong []={"¾¼¾½"};
const char PY_mb_jiu   []={"¾À¾¿¾¾¾Å¾Ã¾Ä¾Á¾Â¾Æ¾É¾Ê¾Ì¾Î¾Ç¾È¾Í¾Ë"};
const char PY_mb_ju    []={"¾Ó¾Ğ¾Ñ¾Ô¾Ò¾Ï¾Ö½Û¾Õ¾×¾Ú¾Ù¾Ø¾ä¾Ş¾Ü¾ß¾æ¾ã¾ç¾å¾İ¾à¾â¾Û¾á"};
const char PY_mb_juan  []={"¾ê¾è¾é¾í¾ë¾î¾ì"};
const char PY_mb_jue   []={"¾ï¾ö¾÷¾ñ¾ø¾ó¾ò¾ô¾ğ"};
const char PY_mb_jun   []={"¾ü¾ı¾ù¾û¾ú¿¡¿¤¾ş¿£¿¥¿¢"};
const char PY_mb_ka    []={"¿§¿¦¿¨"};
const char PY_mb_kai   []={"¿ª¿«¿­¿®¿¬"};
const char PY_mb_kan   []={"¼÷¿¯¿±¿°¿²¿³¿´"};
const char PY_mb_kang  []={"¿µ¿¶¿·¿¸¿º¿¹¿»"};
const char PY_mb_kao   []={"¿¼¿½¿¾¿¿"};
const char PY_mb_ke    []={"¿À¿Á¿Â¿Æ¿Ã¿Å¿Ä¿Ç¿È¿É¿Ê¿Ë¿Ì¿Í¿Î"};
const char PY_mb_ken   []={"¿Ï¿Ñ¿Ò¿Ğ"};
const char PY_mb_keng  []={"¿Ô¿Ó"};
const char PY_mb_kong  []={"¿Õ¿×¿Ö¿Ø"};
const char PY_mb_kou   []={"¿Ù¿Ú¿Û¿Ü"};
const char PY_mb_ku    []={"¿İ¿Ş¿ß¿à¿â¿ã¿á"};
const char PY_mb_kua   []={"¿ä¿å¿æ¿è¿ç"};
const char PY_mb_kuai  []={"¿é¿ì¿ë¿ê"};
const char PY_mb_kuan  []={"¿í¿î"};
const char PY_mb_kuang []={"¿ï¿ğ¿ñ¿ö¿õ¿ó¿ò¿ô"};
const char PY_mb_kui   []={"¿÷¿ù¿ø¿ú¿ü¿û¿ı¿şÀ¢À£À¡"};
const char PY_mb_kun   []={"À¤À¥À¦À§"};
const char PY_mb_kuo   []={"À©À¨À«Àª"};
const char PY_mb_la    []={"À¬À­À²À®À°À¯À±"};
const char PY_mb_lai   []={"À´À³Àµ"};
const char PY_mb_lan   []={"À¼À¹À¸À·À»À¶À¾À½ÀºÀÀÀ¿ÀÂÀÁÀÃÀÄ"};
const char PY_mb_lang  []={"ÀÉÀÇÀÈÀÅÀÆÀÊÀË"};
const char PY_mb_lao   []={"ÀÌÀÍÀÎÀÏÀĞÀÑÀÔÀÓÀÒ"};
const char PY_mb_le    []={"ÀÖÀÕÁË"};
const char PY_mb_lei   []={"À×ÀØÀİÀÚÀÙÀÜÀßÀáÀàÀÛÀŞ"};
const char PY_mb_leng  []={"ÀâÀãÀä"};
const char PY_mb_li    []={"ÀåÀæÀêÀëÀòÀçÀìÁ§ÀèÀéÀñÀîÀïÁ¨ÀíÀğÁ¦ÀúÀ÷Á¢ÀôÀöÀûÀøÁ¤ÀıÁ¥ÀşÀóÀõÀùÁ£ÀüÁ¡"};
const char PY_mb_lian  []={"Á¬Á±Á¯Á°Á«ÁªÁ®Á­Á²Á³Á·Á¶ÁµÁ´"};
const char PY_mb_liang []={"Á©Á¼Á¹ÁºÁ¸Á»Á½ÁÁÁÂÁ¾ÁÀÁ¿"};
const char PY_mb_liao  []={"ÁÊÁÉÁÆÁÄÁÅÁÈÁÎÁÃÁÇÁÍÁÏÁÌ"};
const char PY_mb_lie   []={"ÁĞÁÓÁÒÁÔÁÑ"};
const char PY_mb_lin   []={"ÁÚÁÖÁÙÁÜÁÕÁØÁ×ÁÛÁİÁßÁŞÁà"};
const char PY_mb_ling  []={"ÁæÁéÁëÁáÁèÁåÁêÁçÁâÁãÁäÁìÁîÁí"};
const char PY_mb_liu   []={"ÁïÁõÁ÷ÁôÁğÁòÁóÁñÁöÁøÁù"};
const char PY_mb_long  []={"ÁúÁüÁıÁûÂ¡ÁşÂ¤Â¢Â£"};
const char PY_mb_lou   []={"Â¦Â¥Â§Â¨ÂªÂ©"};
const char PY_mb_lu    []={"Â¶Â¬Â®Â«Â¯Â­Â±Â²Â°Â³Â½Â¼Â¸Â¹Â»ÂµÂ·Â¾ÂºÂ´"};
const char PY_mb_luan  []={"ÂÏÂÍÂÎÂĞÂÑÂÒ"};
const char PY_mb_lue   []={"ÂÓÂÔ"};
const char PY_mb_lun   []={"ÂÕÂØÂ×ÂÙÂÚÂÖÂÛ"};
const char PY_mb_luo   []={"ÂŞÂÜÂßÂàÂáÂâÂİÂãÂåÂçÂæÂä"};
const char PY_mb_lv    []={"ÂËÂ¿ÂÀÂÂÂÃÂÁÂÅÂÆÂÄÂÉÂÇÂÊÂÌÂÈ"};
const char PY_mb_ma    []={"ÂèÂéÂíÂêÂëÂìÂîÂğÂï"};
const char PY_mb_mai   []={"ÂñÂòÂõÂóÂôÂö"};
const char PY_mb_man   []={"ÂùÂøÂ÷ÂúÂüÃ¡ÂıÂşÂû"};
const char PY_mb_mang  []={"Ã¦Ã¢Ã¤Ã£Ã§Ã¥"};
const char PY_mb_mao   []={"Ã¨Ã«Ã¬Ã©ÃªÃ®Ã­Ã¯Ã°Ã³Ã±Ã²"};
const char PY_mb_me    []={"Ã´"};
const char PY_mb_mei   []={"Ã»Ã¶ÃµÃ¼Ã·Ã½ÃºÃ¸Ã¹Ã¿ÃÀÃ¾ÃÃÃÁÃÄÃÂ"};
const char PY_mb_men   []={"ÃÅÃÆÃÇ"};
const char PY_mb_meng  []={"ÃÈÃËÃÊÃÍÃÉÃÌÃÏÃÎ"};
const char PY_mb_mi    []={"ÃÖÃÔÃÕÃÑÃÓÃÒÃ×ÃĞÃÚÃÙÃØÃÜÃİÃÛ"};
const char PY_mb_mian  []={"ÃßÃàÃŞÃâÃãÃäÃáÃåÃæ"};
const char PY_mb_miao  []={"ÃçÃèÃéÃëÃìÃêÃîÃí"};
const char PY_mb_mie   []={"ÃğÃï"};
const char PY_mb_min   []={"ÃñÃóÃòÃöÃõÃô"};
const char PY_mb_ming  []={"ÃûÃ÷ÃùÃúÃøÃü"};
const char PY_mb_miu   []={"Ãı"};
const char PY_mb_mo    []={"ºÑÃşÄ¡Ä£Ä¤Ä¦Ä¥Ä¢Ä§Ä¨Ä©Ä­Ä°ÄªÄ¯Ä®Ä«Ä¬"};
const char PY_mb_mou   []={"Ä²Ä±Ä³"};
const char PY_mb_mu    []={"Ä¸Ä¶ÄµÄ·Ä´Ä¾Ä¿ÄÁÄ¼Ä¹Ä»ÄÀÄ½ÄºÄÂ"};
const char PY_mb_na    []={"ÄÃÄÄÄÇÄÉÄÈÄÆÄÅ"};
const char PY_mb_nai   []={"ÄËÄÌÄÊÄÎÄÍ"};
const char PY_mb_nan   []={"ÄĞÄÏÄÑ"};
const char PY_mb_nang  []={"ÄÒ"};
const char PY_mb_nao   []={"ÄÓÄÕÄÔÄÖÄ×"};
const char PY_mb_ne    []={"ÄØ"};
const char PY_mb_nei   []={"ÄÚÄÙ"};
const char PY_mb_nen   []={"ÄÛ"};
const char PY_mb_neng  []={"ÄÜ"};
const char PY_mb_ni    []={"ÄİÄáÄàÄßÄŞÄãÄâÄæÄäÄçÄå"};
const char PY_mb_nian  []={"ÄéÄêÄíÄìÄëÄîÄè"};
const char PY_mb_niang []={"ÄïÄğ"};
const char PY_mb_niao  []={"ÄñÄò"};
const char PY_mb_nie   []={"ÄóÄùÄôÄöÄ÷ÄøÄõ"};
const char PY_mb_nin   []={"Äú"};
const char PY_mb_ning  []={"ÄşÅ¡ÄüÄûÄıÅ¢"};
const char PY_mb_niu   []={"Å£Å¤Å¦Å¥"};
const char PY_mb_nong  []={"Å©Å¨Å§Åª"};
const char PY_mb_nu    []={"Å«Å¬Å­"};
const char PY_mb_nuan  []={"Å¯"};
const char PY_mb_nue   []={"Å±Å°"};
const char PY_mb_nuo   []={"Å²ÅµÅ³Å´"};
const char PY_mb_nv    []={"Å®"};
const char PY_mb_o     []={"Å¶"};
const char PY_mb_ou    []={"Å·Å¹Å¸Å»Å¼ÅºÅ½"};
const char PY_mb_pa    []={"Å¿Å¾ÅÀ°ÒÅÃÅÁÅÂ"};
const char PY_mb_pai   []={"ÅÄÅÇÅÅÅÆÅÉÅÈ"};
const char PY_mb_pan   []={"ÅËÅÊÅÌÅÍÅĞÅÑÅÎÅÏ"};
const char PY_mb_pang  []={"ÅÒÅÓÅÔÅÕÅÖ"};
const char PY_mb_pao   []={"Å×ÅÙÅØÅÚÅÛÅÜÅİ"};
const char PY_mb_pei   []={"ÅŞÅßÅãÅàÅâÅáÅæÅåÅä"};
const char PY_mb_pen   []={"ÅçÅè"};
const char PY_mb_peng  []={"ÅêÅéÅëÅóÅíÅïÅğÅîÅôÅìÅñÅòÅõÅö"};
const char PY_mb_pi    []={"±ÙÅúÅ÷ÅûÅøÅüÅùÆ¤ÅşÆ£Æ¡ÅıÆ¢Æ¥Æ¦Æ¨Æ§Æ©"};
const char PY_mb_pian  []={"Æ¬Æ«ÆªÆ­"};
const char PY_mb_piao  []={"Æ¯Æ®Æ°Æ±"};
const char PY_mb_pie   []={"Æ²Æ³"};
const char PY_mb_pin   []={"Æ´Æ¶ÆµÆ·Æ¸"};
const char PY_mb_ping  []={"Æ¹Æ½ÆÀÆ¾ÆºÆ»ÆÁÆ¿Æ¼"};
const char PY_mb_po    []={"ÆÂÆÃÆÄÆÅÆÈÆÆÆÉÆÇ"};
const char PY_mb_pou   []={"ÆÊ"};
const char PY_mb_pu    []={"¸¬ÆÍÆËÆÌÆÎÆĞÆÏÆÑÆÓÆÔÆÒÆÖÆÕÆ×ÆØ"};
const char PY_mb_qi    []={"ÆßÆãÆŞÆâÆàÆÜÆİÆÚÆÛÆáÆîÆëÆäÆæÆçÆíÆêÆéÆèÆïÆåÆìÆòÆóÆñÆôÆğÆøÆıÆùÆúÆûÆüÆõÆöÆ÷"};
const char PY_mb_qia   []={"ÆşÇ¡Ç¢"};
const char PY_mb_qian  []={"Ç§ÇªÇ¤Ç¨Ç¥Ç£Ç¦Ç«Ç©Ç°Ç®Ç¯Ç¬Ç±Ç­Ç³Ç²Ç´Ç·ÇµÇ¶Ç¸"};
const char PY_mb_qiang []={"ÇºÇ¼Ç¹Ç»Ç¿Ç½Ç¾ÇÀ"};
const char PY_mb_qiao  []={"ÇÄÇÃÇÂÇÁÇÇÇÈÇÅÇÆÇÉÇÎÇÍÇÏÇÌÇËÇÊ"};
const char PY_mb_qie   []={"ÇĞÇÑÇÒÇÓÇÔ"};
const char PY_mb_qin   []={"Ç×ÇÖÇÕÇÛÇØÇÙÇİÇÚÇÜÇŞÇß"};
const char PY_mb_qing  []={"ÇàÇâÇáÇãÇäÇåÇéÇçÇèÇæÇêÇëÇì"};
const char PY_mb_qiong []={"ÇîÇí"};
const char PY_mb_qiu   []={"ÇğÇñÇïÇôÇóÇöÇõÇò"};
const char PY_mb_qu    []={"ÇøÇúÇıÇüÇùÇûÇ÷ÇşÈ¡È¢È£È¥È¤"};
const char PY_mb_quan  []={"È¦È«È¨ÈªÈ­È¬È©È§È®È°È¯"};
const char PY_mb_que   []={"È²È±È³È´È¸È·ÈµÈ¶"};
const char PY_mb_qun   []={"È¹Èº"};
const char PY_mb_ran   []={"È»È¼È½È¾"};
const char PY_mb_rang  []={"È¿ÈÂÈÀÈÁÈÃ"};
const char PY_mb_rao   []={"ÈÄÈÅÈÆ"};
const char PY_mb_re    []={"ÈÇÈÈ"};
const char PY_mb_ren   []={"ÈËÈÊÈÉÈÌÈĞÈÏÈÎÈÒÈÑÈÍ"};
const char PY_mb_reng  []={"ÈÓÈÔ"};
const char PY_mb_ri    []={"ÈÕ"};
const char PY_mb_rong  []={"ÈÖÈŞÈ×ÈÙÈİÈÜÈØÈÛÈÚÈß"};
const char PY_mb_rou   []={"ÈáÈàÈâ"};
const char PY_mb_ru    []={"ÈçÈãÈåÈæÈäÈêÈéÈèÈëÈì"};
const char PY_mb_ruan  []={"ÈîÈí"};
const char PY_mb_rui   []={"ÈïÈñÈğ"};
const char PY_mb_run   []={"ÈòÈó"};
const char PY_mb_ruo   []={"ÈôÈõ"};
const char PY_mb_sa    []={"ÈöÈ÷Èø"};
const char PY_mb_sai   []={"ÈûÈùÈúÈü"};
const char PY_mb_san   []={"ÈıÈşÉ¡É¢"};
const char PY_mb_sang  []={"É£É¤É¥"};
const char PY_mb_sao   []={"É¦É§É¨É©"};
const char PY_mb_se    []={"É«É¬Éª"};
const char PY_mb_sen   []={"É­"};
const char PY_mb_seng  []={"É®"};
const char PY_mb_sha   []={"É±É³É´É°É¯ÉµÉ¶É·ÏÃ"};
const char PY_mb_shai  []={"É¸É¹"};
const char PY_mb_shan  []={"É½É¾É¼ÉÀÉºÉ¿ÉÁÉÂÉÇÉ»ÉÈÉÆÉÉÉÃÉÅÉÄÕ¤"};
const char PY_mb_shang []={"ÉËÉÌÉÊÉÑÉÎÉÍÉÏÉĞ"};
const char PY_mb_shao  []={"ÉÓÉÒÉÕÉÔÉ×ÉÖÉØÉÙÉÛÉÜÉÚ"};
const char PY_mb_she   []={"ÉİÉŞÉàÉßÉáÉèÉçÉäÉæÉâÉåÉã"};
const char PY_mb_shen  []={"ÉêÉìÉíÉëÉğÉïÉéÉîÉñÉòÉóÉôÉöÉõÉøÉ÷Ê²"};
const char PY_mb_sheng []={"ÉıÉúÉùÉüÊ¤ÉûÉşÊ¡Ê¥Ê¢Ê£"};
const char PY_mb_shi   []={"³×Ê¬Ê§Ê¦Ê­Ê«Ê©Ê¨ÊªÊ®Ê¯Ê±Ê¶ÊµÊ°Ê´Ê³Ê·Ê¸Ê¹Ê¼Ê»ÊºÊ¿ÊÏÊÀÊËÊĞÊ¾Ê½ÊÂÊÌÊÆÊÓÊÔÊÎÊÒÊÑÊÃÊÇÊÁÊÊÊÅÊÍÊÈÊÄÊÉËÆ"};
const char PY_mb_shou  []={"ÊÕÊÖÊØÊ×ÊÙÊÜÊŞÊÛÊÚÊİ"};
const char PY_mb_shu   []={"ÊéÊãÊåÊàÊâÊáÊçÊèÊæÊäÊßÊëÊêÊìÊîÊòÊğÊóÊñÊíÊïÊõÊùÊøÊöÊ÷ÊúË¡ÊüÊıÊûÊşÊô"};
const char PY_mb_shua  []={"Ë¢Ë£"};
const char PY_mb_shuai []={"Ë¥Ë¤Ë¦Ë§"};
const char PY_mb_shuan []={"Ë©Ë¨"};
const char PY_mb_shuang[]={"Ë«ËªË¬"};
const char PY_mb_shui  []={"Ë­Ë®Ë°Ë¯"};
const char PY_mb_shun  []={"Ë±Ë³Ë´Ë²"};
const char PY_mb_shuo  []={"ËµË¸Ë·Ë¶"};
const char PY_mb_si    []={"Ë¿Ë¾Ë½Ë¼Ë¹Ë»ËºËÀËÈËÄËÂËÅËÇËÃËÁ"};
const char PY_mb_song  []={"ËÉËËËÊËÏËÎËĞËÍËÌ"};
const char PY_mb_sou   []={"ËÔËÑËÒËÓ"};
const char PY_mb_su    []={"ËÕËÖË×ËßËàËØËÙËÚËÜËİËÛ"};
const char PY_mb_suan  []={"ËáËâËã"};
const char PY_mb_sui   []={"ËäËçËåËæËèËêËîËìËéËíËë"};
const char PY_mb_sun   []={"ËïËğËñ"};
const char PY_mb_suo   []={"ËôËóËòËõËùË÷ËöËø"};
const char PY_mb_ta    []={"ËıËûËüËúËşÌ¡Ì¢Ì¤Ì£"};
const char PY_mb_tai   []={"Ì¥Ì¨Ì§Ì¦Ì«Ì­Ì¬Ì©Ìª"};
const char PY_mb_tan   []={"Ì®Ì°Ì¯Ì²Ì±Ì³Ì¸ÌµÌ·Ì¶Ì´Ì¹Ì»ÌºÌ¾Ì¿Ì½Ì¼"};
const char PY_mb_tang  []={"ÌÀÌÆÌÃÌÄÌÁÌÂÌÅÌÇÌÈÌÊÌÉÌÌÌË"};
const char PY_mb_tao   []={"ÌÎÌĞÌÍÌÏÌÓÌÒÌÕÌÔÌÑÌÖÌ×"};
const char PY_mb_te    []={"ÌØ"};
const char PY_mb_teng  []={"ÌÛÌÚÌÜÌÙ"};
const char PY_mb_ti    []={"ÌŞÌİÌàÌßÌäÌáÌâÌãÌåÌëÌêÌéÌèÌæÌç"};
const char PY_mb_tian  []={"ÌìÌíÌïÌñÌğÌîÌóÌò"};
const char PY_mb_tiao  []={"µ÷ÌôÌõÌöÌ÷Ìø"};
const char PY_mb_tie   []={"ÌùÌúÌû"};
const char PY_mb_ting  []={"ÌüÍ¡ÌıÌşÍ¢Í¤Í¥Í£Í¦Í§"};
const char PY_mb_tong  []={"Í¨Í¬Í®Í©Í­Í¯ÍªÍ«Í³Í±Í°Í²Í´"};
const char PY_mb_tou   []={"ÍµÍ·Í¶Í¸"};
const char PY_mb_tu    []={"Í¹ÍºÍ»Í¼Í½Í¿Í¾ÍÀÍÁÍÂÍÃ"};
const char PY_mb_tuan  []={"ÍÄÍÅ"};
const char PY_mb_tui   []={"ÍÆÍÇÍÈÍËÍÉÍÊ"};
const char PY_mb_tun   []={"¶ÚÍÌÍÍÍÎ"};
const char PY_mb_tuo   []={"ÍĞÍÏÍÑÍÔÍÓÍÕÍÒÍ×ÍÖÍØÍÙ"};
const char PY_mb_wa    []={"ÍÛÍŞÍÚÍİÍÜÍßÍà"};
const char PY_mb_wai   []={"ÍáÍâ"};
const char PY_mb_wan   []={"ÍäÍåÍãÍèÍêÍæÍçÍéÍğÍìÍíÍñÍïÍîÍëÍòÍó"};
const char PY_mb_wang  []={"ÍôÍöÍõÍøÍùÍ÷ÍıÍüÍúÍû"};
const char PY_mb_wei   []={"Î£ÍşÎ¢Î¡ÎªÎ¤Î§Î¥Î¦Î¨Î©Î¬Î«Î°Î±Î²Î³Î­Î¯Î®ÎÀÎ´Î»Î¶Î·Î¸Î¾Î½Î¹Î¼ÎµÎ¿Îº"};
const char PY_mb_wen   []={"ÎÂÎÁÎÄÎÆÎÅÎÃÎÇÎÉÎÈÎÊ"};
const char PY_mb_weng  []={"ÎÌÎËÎÍ"};
const char PY_mb_wo    []={"ÎÎÎĞÎÑÎÏÎÒÎÖÎÔÎÕÎÓ"};
const char PY_mb_wu    []={"ÎÚÎÛÎØÎ×ÎİÎÜÎÙÎŞÎãÎâÎáÎßÎàÎåÎçÎéÎëÎäÎêÎæÎèÎğÎñÎìÎïÎóÎòÎîÎí"};
const char PY_mb_xi    []={"Ï¦Ï«Î÷ÎüÏ£ÎôÎöÎùÏ¢ÎşÏ¤Ï§Ï©ÎøÎúÏ¬Ï¡ÏªÎıÏ¨ÎõÎûÏ¥Ï°Ï¯Ï®Ï±Ï­Ï´Ï²Ï·ÏµÏ¸Ï¶"};
const char PY_mb_xia   []={"ÏºÏ¹Ï»ÏÀÏ¿ÏÁÏ¾Ï½Ï¼ÏÂÏÅÏÄ"};
const char PY_mb_xian  []={"Ï³ÏÉÏÈÏËÏÆÏÇÏÊÏĞÏÒÏÍÏÌÏÑÏÏÏÎÏÓÏÔÏÕÏØÏÖÏßÏŞÏÜÏİÏÚÏÛÏ×ÏÙ"};
const char PY_mb_xiang []={"ÏçÏàÏãÏáÏæÏäÏåÏâÏêÏéÏèÏíÏìÏëÏòÏïÏîÏóÏñÏğ"};
const char PY_mb_xiao  []={"ÏüÏûÏôÏõÏúÏöÏùÏıĞ¡ÏşĞ¢Ğ¤ÏøĞ§Ğ£Ğ¦Ğ¥"};
const char PY_mb_xie   []={"Ğ©Ğ¨ĞªĞ«Ğ­Ğ°Ğ²Ğ±Ğ³Ğ¯Ğ¬Ğ´Ğ¹ĞºĞ¶Ğ¼ĞµĞ»Ğ¸Ğ·"};
const char PY_mb_xin   []={"ĞÄĞÃĞ¾ĞÁĞÀĞ¿ĞÂĞ½ĞÅĞÆ"};
const char PY_mb_xing  []={"ĞËĞÇĞÊĞÉĞÈĞÌĞÏĞÎĞÍĞÑĞÓĞÕĞÒĞÔ"};
const char PY_mb_xiong []={"Ğ×ĞÖĞÙĞÚĞØĞÛĞÜ"};
const char PY_mb_xiu   []={"ËŞĞİĞŞĞßĞàĞãĞåĞäĞâĞá"};
const char PY_mb_xu    []={"ĞçĞëĞéĞêĞèĞæĞìĞíĞñĞòĞğĞôĞ÷ĞøĞïĞöĞõĞîÓõ"};
const char PY_mb_xuan  []={"ĞùĞûĞúĞşĞüĞıÑ¡Ñ¢Ñ¤Ñ£"};
const char PY_mb_xue   []={"Ï÷Ñ¥Ñ¦Ñ¨Ñ§Ñ©Ñª"};
const char PY_mb_xun   []={"Ñ«Ñ¬Ñ°Ñ²Ñ®Ñ±Ñ¯Ñ­ÑµÑ¶Ñ´Ñ¸Ñ·Ñ³"};
const char PY_mb_ya    []={"Ñ¾Ñ¹Ñ½ÑºÑ»Ñ¼ÑÀÑ¿ÑÁÑÂÑÄÑÃÑÆÑÅÑÇÑÈ"};
const char PY_mb_yan   []={"ÑÊÑÌÑÍÑÉÑËÑÓÑÏÑÔÑÒÑØÑ×ÑĞÑÎÑÖÑÑÑÕÑÙÑÜÑÚÑÛÑİÑáÑåÑâÑäÑçÑŞÑéÑèÑßÑæÑãÑà"};
const char PY_mb_yang  []={"ÑëÑêÑíÑìÑïÑòÑôÑîÑğÑñÑóÑöÑøÑõÑ÷ÑùÑú"};
const char PY_mb_yao   []={"½ÄÑıÑüÑûÒ¢Ò¦Ò¤Ò¥Ò¡Ò£ÑşÒ§Ò¨Ò©ÒªÒ«Ô¿"};
const char PY_mb_ye    []={"Ò¬Ò­Ò¯Ò®Ò²Ò±Ò°ÒµÒ¶Ò·Ò³Ò¹Ò´ÒºÒ¸"};
const char PY_mb_yi    []={"Ò»ÒÁÒÂÒ½ÒÀÒ¿Ò¼Ò¾ÒÇÒÄÒÊÒËÒÌÒÈÒÆÒÅÒÃÒÉÒÍÒÒÒÑÒÔÒÓÒÏÒĞÒÎÒåÒÚÒäÒÕÒéÒàÒÙÒìÒÛÒÖÒëÒØÒ×ÒïÒèÒßÒæÒêÒîÒİÒâÒçÒŞÒáÒãÒíÒÜ"};
const char PY_mb_yin   []={"ÒòÒõÒöÒğÒñÒôÒóÒ÷ÒúÒùÒøÒüÒıÒûÒşÓ¡"};
const char PY_mb_ying  []={"Ó¦Ó¢Ó¤Ó§Ó£Ó¥Ó­Ó¯Ó«Ó¨Ó©ÓªÓ¬Ó®Ó±Ó°Ó³Ó²"};
const char PY_mb_yo    []={"Ó´"};
const char PY_mb_yong  []={"Ó¶ÓµÓ¸Ó¹ÓºÓ·ÓÀÓ½Ó¾ÓÂÓ¿ÓÁÓ¼Ó»ÓÃ"};
const char PY_mb_you   []={"ÓÅÓÇÓÄÓÆÓÈÓÉÓÌÓÊÓÍÓËÓÎÓÑÓĞÓÏÓÖÓÒÓ×ÓÓÓÕÓÔ"};
const char PY_mb_yu    []={"ÓØÓÙÓåÓÚÓèÓàÓÛÓãÓáÓéÓæÓçÓäÓâÓŞÓÜÓİÓßÓëÓîÓìÓğÓêÓíÓïÓñÔ¦ÓóÓıÓôÓüÓøÔ¡Ô¤ÓòÓûÓ÷Ô¢ÓùÔ£ÓöÓúÓşÔ¥"};
const char PY_mb_yuan  []={"Ô©Ô§Ô¨ÔªÔ±Ô°Ô«Ô­Ô²Ô¬Ô®ÔµÔ´Ô³Ô¯Ô¶Ô·Ô¹ÔºÔ¸"};
const char PY_mb_yue   []={"Ô»Ô¼ÔÂÔÀÔÃÔÄÔ¾ÔÁÔ½"};
const char PY_mb_yun   []={"ÔÆÔÈÔÇÔÅÔÊÔÉÔĞÔËÔÎÔÍÔÏÔÌ"};
const char PY_mb_za    []={"ÔÑÔÓÔÒÕ¦"};
const char PY_mb_zai   []={"ÔÖÔÕÔÔÔ×ÔØÔÙÔÚ×Ğ"};
const char PY_mb_zan   []={"ÔÛÔÜÔİÔŞ"};
const char PY_mb_zang  []={"ÔßÔàÔá"};
const char PY_mb_zao   []={"ÔâÔãÔäÔçÔæÔéÔèÔåÔîÔíÔìÔëÔïÔê"};
const char PY_mb_ze    []={"ÔòÔñÔóÔğ"};
const char PY_mb_zei   []={"Ôô"};
const char PY_mb_zen   []={"Ôõ"};
const char PY_mb_zeng  []={"ÔöÔ÷Ôù"};
const char PY_mb_zha   []={"ÔûÔüÔúÔıÔşÕ¢Õ¡Õ£Õ§Õ©Õ¨Õ¥×õ"};
const char PY_mb_zhai  []={"Õ«ÕªÕ¬µÔÕ­Õ®Õ¯"};
const char PY_mb_zhan  []={"Õ´Õ±Õ³Õ²Õ°Õ¶Õ¹ÕµÕ¸Õ·Õ¼Õ½Õ»Õ¾ÕÀÕ¿Õº"};
const char PY_mb_zhang []={"³¤ÕÅÕÂÕÃÕÄÕÁÕÇÕÆÕÉÕÌÕÊÕÈÕÍÕËÕÏÕÎ"};
const char PY_mb_zhao  []={"ÕĞÕÑÕÒÕÓÕÙÕ×ÕÔÕÕÕÖÕØ×¦"};
const char PY_mb_zhe   []={"ÕÚÕÛÕÜÕİÕŞÕßÕàÕâÕãÕá×Å"};
const char PY_mb_zhen  []={"ÕêÕëÕìÕäÕæÕèÕåÕçÕéÕïÕíÕîÕóÕñÕòÕğÖ¡"};
const char PY_mb_zheng []={"ÕùÕ÷ÕúÕõÕøÕöÕôÕüÕûÕıÖ¤Ö£ÕşÖ¢"};
const char PY_mb_zhi   []={"Ö®Ö§Ö­Ö¥Ö¨Ö¦ÖªÖ¯Ö«Ö¬Ö©Ö´Ö¶Ö±ÖµÖ°Ö²Ö³Ö¹Ö»Ö¼Ö·Ö½Ö¸ÖºÖÁÖ¾ÖÆÖÄÖÎÖËÖÊÖÅÖ¿ÖÈÖÂÖÀÖÌÖÏÖÇÖÍÖÉÖÃ"};
const char PY_mb_zhong []={"ÖĞÖÒÖÕÖÑÖÓÖÔÖ×ÖÖÖÙÖÚÖØ"};
const char PY_mb_zhou  []={"ÖİÖÛÖßÖÜÖŞÖàÖáÖâÖãÖäÖæÖçÖåÖè"};
const char PY_mb_zhu   []={"ÖìÖïÖêÖéÖîÖíÖëÖñÖòÖğÖ÷ÖôÖóÖöÖõ×¡Öú×¢Öü×¤Öù×£ÖøÖûÖşÖı"};
const char PY_mb_zhua  []={"×¥"};
const char PY_mb_zhuai []={"×§"};
const char PY_mb_zhuan []={"×¨×©×ª×«×­"};
const char PY_mb_zhuang[]={"×±×¯×®×°×³×´´±×²"};
const char PY_mb_zhui  []={"×·×µ×¶×¹×º×¸"};
const char PY_mb_zhun  []={"×»×¼"};
const char PY_mb_zhuo  []={"×¿×¾×½×À×Æ×Â×Ç×Ã×Ä×Á"};
const char PY_mb_zi    []={"×Î×È×É×Ë×Ê×Í×Ì×Ñ×Ó×Ï×Ò×Ö×Ô×Õ"};
const char PY_mb_zong  []={"×Ú×Û×Ø×Ù×××Ü×İ"};
const char PY_mb_zou   []={"×Ş×ß×à×á"};
const char PY_mb_zu    []={"×â×ã×ä×å×ç×è×é×æ"};
const char PY_mb_zuan  []={"×¬×ë×ê"};
const char PY_mb_zui   []={"×ì×î×ï×í"};
const char PY_mb_zun   []={"×ğ×ñ"};
const char PY_mb_zuo   []={"×ò×ó×ô×÷×ø×ù×ö"};
const char PY_mb_none  []={""};

//================================================================================================


//"Æ´ÒôÊäÈë·¨²éÑ¯Âë±í"
//typedef struct                  //
//{
//       const char* YunMu;               // 
//       uint8 offset_low;                       //
//       uint8 offset_high;                       //
//}typPinYin;

typedef struct                  //
{
       const char* YunMu;               // 
	   const char* MB;
}typPinYin;


//const typPinYin PY_index_a[]={
//  							{"",0x00,0x00},
//  							{"i",0x05,0x00},
//  							{"n",0x20,0x00},
//  							{"ng",0x33,0x00},
//  							{"o",0x3A,0x00}};
//const typPinYin PY_index_b[]={
//  							{"a",0x4D,0x00},
//  							{"ai",0x70,0x00},
//  							{"an",0x81,0x00},
//  							{"ang",0xA0,0x00},
//  							{"ao",0xB9,0x00},
//  							{"ei",0xDE,0x00},
//  							{"en",0xFD,0x00},
//  							{"eng",0x08,0x01},
//  							{"i",0x15,0x01},
//  							{"ian",0x44,0x01},
//  							{"iao",0x5D,0x01},
//  							{"ie",0x66,0x01},
//  							{"in",0x6F,0x01},
//  							{"ing",0x7C,0x01},
//  							{"o",0x8F,0x01},
//  							{"u",0xB8,0x01}};
//const typPinYin PY_index_c[]={
//  							{"a",0xCD,0x01},
//  							{"ai",0xD0,0x01},
//  							{"an",0xE7,0x01},
//  							{"ang",0xF6,0x01},
//  							{"ao",0x01,0x02},
//  							{"e",0x0C,0x02},
//  							{"eng",0x17,0x02},
//  							{"ha",0x1E,0x02},
//  							{"hai",0x37,0x02},
//  							{"han",0x3E,0x02},
//  							{"hang",0x53,0x02},
//  							{"hao",0x6C,0x02},
//  							{"he",0x81,0x02},
//  							{"hen",0x8E,0x02},
//  							{"heng",0xA3,0x02},
//  							{"hi",0xC2,0x02},
//  							{"hong",0xE1,0x02},
//  							{"hou",0xEC,0x02},
//  							{"hu",0x05,0x03},
//  							{"huai",0x28,0x03},
//  							{"huan",0x2B,0x03},
//  							{"huang",0x3A,0x03},
//  							{"hui",0x45,0x03},
//  							{"hun",0x50,0x03},
//  							{"huo",0x5F,0x03},
//  							{"i",0x62,0x03},
//  							{"ong",0x7B,0x03},
//  							{"ou",0x88,0x03},
//  							{"u",0x8B,0x03},
//  							{"uan",0x94,0x03},
//  							{"ui",0x9B,0x03},
//  							{"un",0xAC,0x03},
//  							{"uo",0xB3,0x03}};
//const typPinYin PY_index_d[]={
//  							{"a",0xC0,0x03},
//  							{"ai",0xCD,0x03},
//  							{"an",0xE6,0x03},
//  							{"ang",0x05,0x04},
//  							{"ao",0x10,0x04},
//  							{"e",0x29,0x04},
//  							{"eng",0x30,0x04},
//  							{"i",0x3F,0x04},
//  							{"ian",0x64,0x04},
//  							{"iao",0x85,0x04},
//  							{"ie",0x96,0x04},
//  							{"ing",0xA5,0x04},
//  							{"iu",0xB8,0x04},
//  							{"ong",0xBB,0x04},
//  							{"ou",0xD0,0x04},
//  							{"u",0xE1,0x04},
//  							{"uan",0xFE,0x04},
//  							{"ui",0x0B,0x05},
//  							{"un",0x14,0x05},
//  							{"uo",0x25,0x05}};
//const typPinYin PY_index_e[]={
//  							{" ",0x3E,0x05},
//  							{"n",0x59,0x05},
//  							{"r",0x5C,0x05}};
//const typPinYin PY_index_f[]={
//  							{"a",0x6D,0x05},
//  							{"an",0x7E,0x05},
//  							{"ang",0xA1,0x05},
//  							{"ei",0xB8,0x05},
//  							{"en",0xD1,0x05},
//  							{"eng",0xF0,0x05},
//  							{"o",0x0F,0x06},
//  							{"ou",0x12,0x06},
//  							{"u",0x15,0x06}};
//const typPinYin PY_index_g[]={
//  							{"a",0x6E,0x06},
//  							{"ai",0x73,0x06},
//  							{"an",0x80,0x06},
//  							{"ang",0x97,0x06},
//  							{"ao",0xAA,0x06},
//  							{"e",0xBF,0x06},
//  							{"ei",0xE2,0x06},
//  							{"en",0xE5,0x06},
//  							{"eng",0xEA,0x06},
//  							{"ong",0xF9,0x06},
//  							{"ou",0x18,0x07},
//  							{"u",0x2B,0x07},
//  							{"ua",0x50,0x07},
//  							{"uai",0x5D,0x07},
//  							{"uan",0x64,0x07},
//  							{"uang",0x7B,0x07},
//  							{"ui",0x82,0x07},
//  							{"un",0xA3,0x07},
//  							{"uo",0xAA,0x07}};
//const typPinYin PY_index_h[]={
//  							{"a",0xB7,0x07},
//  							{"ai",0xBC,0x07},
//  							{"an",0xCB,0x07},
//  							{"ang",0xF2,0x07},
//  							{"ao",0xF9,0x07},
//  							{"e",0x0C,0x08},
//  							{"ei",0x2F,0x08},
//  							{"en",0x34,0x08},
//  							{"eng",0x3D,0x08},
//  							{"ong",0x48,0x08},
//  							{"ou",0x5B,0x08},
//  							{"u",0x6A,0x08},
//  							{"ua",0x8F,0x08},
//  							{"uai",0xA2,0x08},
//  							{"uan",0xAD,0x08},
//  							{"uang",0xCA,0x08},
//  							{"ui",0xE7,0x08},
//  							{"un",0x12,0x09},
//  							{"uo",0x1F,0x09}};
//const typPinYin PY_index_j[]={
//  							{"i",0x34,0x09},
//  							{"ia",0xA1,0x09},
//  							{"ian",0xC6,0x09},
//  							{"iang",0x15,0x0A},
//  							{"iao",0x30,0x0A},
//  							{"ie",0x69,0x0A},
//  							{"in",0x9C,0x0A},
//  							{"ing",0xC5,0x0A},
//  							{"iong",0xF8,0x0A},
//  							{"iu",0xFD,0x0A},
//  							{"u",0x20,0x0B},
//  							{"uan",0x55,0x0B},
//  							{"ue",0x64,0x0B},
//  							{"un",0x77,0x0B}};
//const typPinYin PY_index_k[]={
//  							{"a",0x8E,0x0B},
//  							{"ai",0x95,0x0B},
//  							{"an",0xA0,0x0B},
//  							{"ang",0xAF,0x0B},
//  							{"ao",0xBE,0x0B},
//  							{"e",0xC7,0x0B},
//  							{"en",0xE6,0x0B},
//  							{"eng",0xEF,0x0B},
//  							{"ong",0xF4,0x0B},
//  							{"ou",0xFD,0x0B},
//  							{"u",0x06,0x0C},
//  							{"ua",0x15,0x0C},
//  							{"uai",0x20,0x0C},
//  							{"uan",0x29,0x0C},
//  							{"uang",0x2E,0x0C},
//  							{"ui",0x3F,0x0C},
//  							{"un",0x56,0x0C},
//  							{"uo",0x5F,0x0C}};
//const typPinYin PY_index_l[]={
//  							{"a",0x68,0x0C},
//  							{"ai",0x77,0x0C},
//  							{"an",0x7E,0x0C},
//  							{"ang",0x9D,0x0C},
//  							{"ao",0xAC,0x0C},
//  							{"e",0xBF,0x0C},
//  							{"ei",0xC6,0x0C},
//  							{"eng",0xDD,0x0C},
//  							{"i",0xE4,0x0C},
//  							{"ian",0x29,0x0D},
//  							{"iang",0x46,0x0D},
//  							{"iao",0x5F,0x0D},
//  							{"ie",0x78,0x0D},
//  							{"in",0x83,0x0D},
//  							{"ing",0x9C,0x0D},
//  							{"iu",0xB9,0x0D},
//  							{"ong",0xD0,0x0D},
//  							{"ou",0xE3,0x0D},
//  							{"u",0xF0,0x0D},
//  							{"uan",0x19,0x0E},
//  							{"ue",0x26,0x0E},
//  							{"un",0x2B,0x0E},
//  							{"uo",0x3A,0x0E},
//  							{"v",0x53,0x0E}};
//const typPinYin PY_index_m[]={
//  							{"a",0x70,0x0E},
//  							{"ai",0x83,0x0E},
//  							{"an",0x90,0x0E},
//  							{"ang",0xA3,0x0E},
//  							{"ao",0xB0,0x0E},
//  							{"e",0xC9,0x0E},
//  							{"ei",0xCC,0x0E},
//  							{"en",0xED,0x0E},
//  							{"eng",0xF4,0x0E},
//  							{"i",0x05,0x0F},
//  							{"ian",0x22,0x0F},
//  							{"iao",0x35,0x0F},
//  							{"ie",0x46,0x0F},
//  							{"in",0x4B,0x0F},
//  							{"ing",0x58,0x0F},
//  							{"iu",0x65,0x0F},
//  							{"o",0x68,0x0F},
//  							{"ou",0x8D,0x0F},
//  							{"u",0x94,0x0F}};
//const typPinYin PY_index_n[]={
//  							{"a",0xB3,0x0F},
//  							{"ai",0xC2,0x0F},
//  							{"an",0xCD,0x0F},
//  							{"ang",0xD4,0x0F},
//  							{"ao",0xD7,0x0F},
//  							{"e",0xE2,0x0F},
//  							{"ei",0xE5,0x0F},
//  							{"en",0xEA,0x0F},
//  							{"eng",0xED,0x0F},
//  							{"i",0xF0,0x0F},
//  							{"ian",0x07,0x10},
//  							{"iang",0x16,0x10},
//  							{"iao",0x1B,0x10},
//  							{"ie",0x20,0x10},
//  							{"in",0x2F,0x10},
//  							{"ing",0x32,0x10},
//  							{"iu",0x3F,0x10},
//  							{"ong",0x48,0x10},
//  							{"u",0x51,0x10},
//  							{"uan",0x58,0x10},
//  							{"ue",0x5B,0x10},
//  							{"uo",0x60,0x10},
//  							{"v",0x69,0x10}};
//const typPinYin PY_index_o[]={
//  							{" ",0x6C,0x10},
//  							{"u",0x6F,0x10}};
//const typPinYin PY_index_p[]={
//  							{"a",0x7E,0x10},
//  							{"ai",0x8D,0x10},
//  							{"an",0x9A,0x10},
//  							{"ang",0xAB,0x10},
//  							{"ao",0xB6,0x10},
//  							{"ei",0xC5,0x10},
//  							{"en",0xD8,0x10},
//  							{"eng",0xDD,0x10},
//  							{"i",0xFA,0x10},
//  							{"ian",0x1F,0x11},
//  							{"iao",0x28,0x11},
//  							{"ie",0x31,0x11},
//  							{"in",0x36,0x11},
//  							{"ing",0x41,0x11},
//  							{"o",0x54,0x11},
//  							{"ou",0x65,0x11},
//  							{"u",0x68,0x11}};
//const typPinYin PY_index_q[]={
//  							{"i",0x87,0x11},
//  							{"ia",0xD0,0x11},
//  							{"ian",0xD7,0x11},
//  							{"iang",0x04,0x12},
//  							{"iao",0x15,0x12},
//  							{"ie",0x34,0x12},
//  							{"in",0x3F,0x12},
//  							{"ing",0x56,0x12},
//  							{"iong",0x71,0x12},
//  							{"iu",0x76,0x12},
//  							{"u",0x87,0x12},
//  							{"uan",0xA2,0x12},
//  							{"ue",0xB9,0x12},
//  							{"un",0xCA,0x12}};
//const typPinYin PY_index_r[]={
//  							{"an",0xCF,0x12},
//  							{"ang",0xD8,0x12},
//  							{"ao",0xE3,0x12},
//  							{"e",0xEA,0x12},
//  							{"en",0xEF,0x12},
//  							{"eng",0x04,0x13},
//  							{"i",0x09,0x13},
//  							{"ong",0x0C,0x13},
//  							{"ou",0x21,0x13},
//  							{"u",0x28,0x13},
//  							{"uan",0x3D,0x13},
//  							{"ui",0x42,0x13},
//  							{"un",0x49,0x13},
//  							{"uo",0x4E,0x13}};
//const typPinYin PY_index_s[]={
//  							{"a",0x53,0x13},
//  							{"ai",0x5A,0x13},
//  							{"an",0x63,0x13},
//  							{"ang",0x6C,0x13},
//  							{"ao",0x73,0x13},
//  							{"e",0x7C,0x13},
//  							{"en",0x83,0x13},
//  							{"eng",0x86,0x13},
//  							{"ha",0x89,0x13},
//  							{"hai",0x9C,0x13},
//  							{"han",0xA1,0x13},
//  							{"hang",0xC4,0x13},
//  							{"hao",0xD5,0x13},
//  							{"he",0xEC,0x13},
//  							{"hen",0x05,0x14},
//  							{"heng",0x28,0x14},
//  							{"hi",0x3F,0x14},
//  							{"hou",0xA0,0x14},
//  							{"hu",0xB5,0x14},
//  							{"hua",0xF8,0x14},
//  							{"huai",0xFD,0x14},
//  							{"huan",0x06,0x15},
//  							{"huang",0x0B,0x15},
//  							{"hui",0x12,0x15},
//  							{"hun",0x1B,0x15},
//  							{"huo",0x24,0x15},
//  							{"i",0x2D,0x15},
//  							{"ong",0x4C,0x15},
//  							{"ou",0x5D,0x15},
//  							{"u",0x66,0x15},
//  							{"uan",0x7D,0x15},
//  							{"ui",0x84,0x15},
//  							{"un",0x9B,0x15},
//  							{"uo",0xA2,0x15}};
//const typPinYin PY_index_t[]={
//  							{"a",0xB3,0x15},
//  							{"ai",0xC6,0x15},
//  							{"an",0xD9,0x15},
//  							{"ang",0xFE,0x15},
//  							{"ao",0x19,0x16},
//  							{"e",0x30,0x16},
//  							{"eng",0x33,0x16},
//  							{"i",0x3C,0x16},
//  							{"ian",0x5B,0x16},
//  							{"iao",0x6C,0x16},
//  							{"ie",0x79,0x16},
//  							{"ing",0x80,0x16},
//  							{"ong",0x95,0x16},
//  							{"ou",0xB0,0x16},
//  							{"u",0xB9,0x16},
//  							{"uan",0xD0,0x16},
//  							{"ui",0xD5,0x16},
//  							{"un",0xE2,0x16},
//  							{"uo",0xEB,0x16}};
//const typPinYin PY_index_w[]={
//  							{"a",0x02,0x17},
//  							{"ai",0x11,0x17},
//  							{"an",0x16,0x17},
//  							{"ang",0x39,0x17},
//  							{"ei",0x4E,0x17},
//  							{"en",0x91,0x17},
//  							{"eng",0xA6,0x17},
//  							{"o",0xAD,0x17},
//  							{"u",0xC0,0x17}};
//const typPinYin PY_index_x[]={
//  							{"i",0xFB,0x17},
//  							{"ia",0x40,0x18},
//  							{"ian",0x59,0x18},
//  							{"iang",0x90,0x18},
//  							{"iao",0xB9,0x18},
//  							{"ie",0xDC,0x18},
//  							{"in",0x05,0x19},
//  							{"ing",0x1A,0x19},
//  							{"iong",0x37,0x19},
//  							{"iu",0x46,0x19},
//  							{"u",0x5B,0x19},
//  							{"uan",0x82,0x19},
//  							{"ue",0x97,0x19},
//  							{"un",0xA6,0x19}};
//const typPinYin PY_index_y[]={
//  							{"a",0xC3,0x19},
//  							{"an",0xE4,0x19},
//  							{"ang",0x27,0x1A},
//  							{"ao",0x4A,0x1A},
//  							{"e",0x6D,0x1A},
//  							{"i",0x8C,0x1A},
//  							{"in",0xF7,0x1A},
//  							{"ing",0x18,0x1B},
//  							{"o",0x3D,0x1B},
//  							{"ong",0x40,0x1B},
//  							{"ou",0x5F,0x1B},
//  							{"u",0x88,0x1B},
//  							{"uan",0xE1,0x1B},
//  							{"ue",0x0A,0x1C},
//  							{"un",0x1D,0x1C}};
//const typPinYin PY_index_z[]={
//  							{"a",0x36,0x1C},
//  							{"ai",0x3F,0x1C},
//  							{"an",0x50,0x1C},
//  							{"ang",0x59,0x1C},
//  							{"ao",0x60,0x1C},
//  							{"e",0x7D,0x1C},
//  							{"ei",0x86,0x1C},
//  							{"en",0x89,0x1C},
//  							{"eng",0x8C,0x1C},
//  							{"ha",0x93,0x1C},
//  							{"hai",0xAE,0x1C},
//  							{"han",0xBD,0x1C},
//  							{"hang",0xE0,0x1C},
//  							{"hao",0x01,0x1D},
//  							{"he",0x18,0x1D},
//  							{"hen",0x2F,0x1D},
//  							{"heng",0x52,0x1D},
//  							{"hi",0x6F,0x1D},
//  							{"hong",0xC6,0x1D},
//  							{"hou",0xDD,0x1D},
//  							{"hu",0xFA,0x1D},
//  							{"hua",0x2F,0x1E},
//  							{"huai",0x32,0x1E},
//  							{"huan",0x35,0x1E},
//  							{"huang",0x40,0x1E},
//  							{"hui",0x51,0x1E},
//  							{"hun",0x5E,0x1E},
//  							{"huo",0x63,0x1E},
//  							{"i",0x78,0x1E},
//  							{"ong",0x95,0x1E},
//  							{"ou",0xA4,0x1E},
//  							{"u",0xAD,0x1E},
//  							{"uan",0xBE,0x1E},
//  							{"ui",0xC5,0x1E},
//  							{"un",0xCE,0x1E},
//  							{"uo",0xD3,0x1E}};
//const typPinYin PY_index_end[]={" ",0,0};



const typPinYin PY_index_a[]={
  							{"",PY_mb_a},
  							{"i",PY_mb_ai},
  							{"n",PY_mb_an},
  							{"ng",PY_mb_ang},
  							{"o",PY_mb_ao}};
const typPinYin PY_index_b[]={
  							{"a",PY_mb_ba},
  							{"ai",PY_mb_bai},
  							{"an",PY_mb_ban},
  							{"ang",PY_mb_bang},
  							{"ao",PY_mb_bao},
  							{"ei",PY_mb_bei},
  							{"en",PY_mb_ben},
  							{"eng",PY_mb_beng},
  							{"i",PY_mb_bi},
  							{"ian",PY_mb_bian},
  							{"iao",PY_mb_biao},
  							{"ie",PY_mb_bie},
  							{"in",PY_mb_bin},
  							{"ing",PY_mb_bing},
  							{"o",PY_mb_bo},
  							{"u",PY_mb_bu}};
const typPinYin PY_index_c[]={
  							{"a",PY_mb_ca},
  							{"ai",PY_mb_cai},
  							{"an",PY_mb_can},
  							{"ang",PY_mb_cang},
  							{"ao",PY_mb_cao},
  							{"e",PY_mb_ce},
  							{"eng",PY_mb_ceng},
  							{"ha",PY_mb_cha},
  							{"hai",PY_mb_chai},
  							{"han",PY_mb_chan},
  							{"hang",PY_mb_chang},
  							{"hao",PY_mb_chao},
  							{"he",PY_mb_che},
  							{"hen",PY_mb_chen},
  							{"heng",PY_mb_cheng},
  							{"hi",PY_mb_chi},
  							{"hong",PY_mb_chong},
  							{"hou",PY_mb_chou},
  							{"hu",PY_mb_chu},
  							{"huai",PY_mb_chuai},
  							{"huan",PY_mb_chuan},
  							{"huang",PY_mb_chuang},
  							{"hui",PY_mb_chui},
  							{"hun",PY_mb_chun},
  							{"huo",	PY_mb_chuo},
  							{"i",	PY_mb_ci},
  							{"ong",	PY_mb_cong},
  							{"ou",	PY_mb_cou},
  							{"u",	PY_mb_cu},
  							{"uan",	PY_mb_cuan},
  							{"ui",	PY_mb_cui},
  							{"un",	PY_mb_cun},
  							{"uo",	PY_mb_cuo}};
const typPinYin PY_index_d[]={
  							{"a",	PY_mb_da},
  							{"ai",	PY_mb_dai},
  							{"an",	PY_mb_dan},
  							{"ang",	PY_mb_dang},
  							{"ao",	PY_mb_dao},
  							{"e",	PY_mb_de},
  							{"eng",	PY_mb_deng},
  							{"i",	PY_mb_di},
  							{"ian",	PY_mb_dian},
  							{"iao",	PY_mb_diao},
  							{"ie",	PY_mb_die},
  							{"ing",	PY_mb_ding},
  							{"iu",	PY_mb_diu},
  							{"ong",	PY_mb_dong},
  							{"ou",	PY_mb_dou},
  							{"u",	PY_mb_du},
  							{"uan",	PY_mb_duan},
  							{"ui",	PY_mb_dui},
  							{"un",	PY_mb_dun},
  							{"uo",	PY_mb_duo}};
const typPinYin PY_index_e[]={
  							{" ",	PY_mb_e},
  							{"n",	PY_mb_en},
  							{"r",	PY_mb_er}};
const typPinYin PY_index_f[]={
  							{"a",	PY_mb_fa},
  							{"an",	PY_mb_fan},
  							{"ang",	PY_mb_fang},
  							{"ei",	PY_mb_fei},
  							{"en",	PY_mb_fen},
  							{"eng",	PY_mb_feng},
  							{"o",	PY_mb_fo},
  							{"ou",	PY_mb_fou},
  							{"u",	PY_mb_fu}};
const typPinYin PY_index_g[]={
  							{"a",	PY_mb_ga},
  							{"ai",	PY_mb_gai},
  							{"an",	PY_mb_gan},
  							{"ang",	PY_mb_gang},
  							{"ao",	PY_mb_gao},
  							{"e",	PY_mb_ge},
  							{"ei",	PY_mb_gei},
  							{"en",	PY_mb_gen},
  							{"eng",	PY_mb_geng},
  							{"ong",	PY_mb_gong},
  							{"ou",	PY_mb_gou},
  							{"u",	PY_mb_gu},
  							{"ua",	PY_mb_gua},
  							{"uai",	PY_mb_guai},
  							{"uan",	PY_mb_guan},
  							{"uang",PY_mb_guang},
  							{"ui",	PY_mb_gui},
  							{"un",	PY_mb_gun},
  							{"uo",	PY_mb_guo}};
const typPinYin PY_index_h[]={
  							{"a",	PY_mb_ha},
  							{"ai",	PY_mb_hai},
  							{"an",	PY_mb_han},
  							{"ang",	PY_mb_hang},
  							{"ao",	PY_mb_hao},
  							{"e",	PY_mb_he},
  							{"ei",	PY_mb_hei},
  							{"en",	PY_mb_hen},
  							{"eng",	PY_mb_heng},
  							{"ong",	PY_mb_hong},
  							{"ou",	PY_mb_hou},
  							{"u",	PY_mb_hu},
  							{"ua",	PY_mb_hua},
  							{"uai",	PY_mb_huai},
  							{"uan",	PY_mb_huan},
  							{"uang",PY_mb_huang},
  							{"ui",	PY_mb_hui},
  							{"un",	PY_mb_hun},
  							{"uo",	PY_mb_huo}};
const typPinYin PY_index_j[]={
  							{"i",	PY_mb_ji},
  							{"ia",	PY_mb_jia},
  							{"ian",	PY_mb_jian},
  							{"iang",PY_mb_jiang},
  							{"iao",	PY_mb_jiao},
  							{"ie",	PY_mb_jie},
  							{"in",	PY_mb_jin},
  							{"ing",	PY_mb_jing},
  							{"iong",PY_mb_jiong},
  							{"iu",	PY_mb_jiu},
  							{"u",	PY_mb_ju},
  							{"uan",	PY_mb_juan},
  							{"ue",	PY_mb_jue},
  							{"un",	PY_mb_jun}};
const typPinYin PY_index_k[]={
  							{"a",	PY_mb_ka},
  							{"ai",	PY_mb_kai},
  							{"an",	PY_mb_kan},
  							{"ang",	PY_mb_kang},
  							{"ao",	PY_mb_kao},
  							{"e",	PY_mb_ke},
  							{"en",	PY_mb_ken},
  							{"eng",	PY_mb_keng},
  							{"ong",	PY_mb_kong},
  							{"ou",	PY_mb_kou},
  							{"u",	PY_mb_ku},
  							{"ua",	PY_mb_kua},
  							{"uai",	PY_mb_kuai},
  							{"uan",	PY_mb_kuan},
  							{"uang",PY_mb_kuang},
  							{"ui",	PY_mb_kui},
  							{"un",	PY_mb_kun},
  							{"uo",	PY_mb_kuo}};
const typPinYin PY_index_l[]={
  							{"a",	PY_mb_la},
  							{"ai",	PY_mb_lai},
  							{"an",	PY_mb_lan},
  							{"ang",	PY_mb_lang},
  							{"ao",	PY_mb_lao},
  							{"e",	PY_mb_le},
  							{"ei",	PY_mb_lei},
  							{"eng",	PY_mb_leng},
  							{"i",	PY_mb_li},
  							{"ian",	PY_mb_lian},
  							{"iang",PY_mb_liang},
  							{"iao",	PY_mb_liao},
  							{"ie",	PY_mb_lie},
  							{"in",	PY_mb_lin},
  							{"ing",	PY_mb_ling},
  							{"iu",	PY_mb_liu},
  							{"ong",	PY_mb_long},
  							{"ou",	PY_mb_lou},
  							{"u",	PY_mb_lu},
  							{"uan",	PY_mb_luan},
  							{"ue",	PY_mb_lue},
  							{"un",	PY_mb_lun},
  							{"uo",	PY_mb_luo},
  							{"v",	PY_mb_lv}};
const typPinYin PY_index_m[]={
  							{"a",	PY_mb_ma},
  							{"ai",	PY_mb_ma},
  							{"an",	PY_mb_man},
  							{"ang",	PY_mb_mang},
  							{"ao",	PY_mb_mao},
  							{"e",	PY_mb_me},
  							{"ei",	PY_mb_mei},
  							{"en",	PY_mb_men},
  							{"eng",	PY_mb_meng},
  							{"i",	PY_mb_mi},
  							{"ian",	PY_mb_mian},
  							{"iao",	PY_mb_miao},
  							{"ie",	PY_mb_mie},
  							{"in",	PY_mb_min},
  							{"ing",	PY_mb_ming},
  							{"iu",	PY_mb_miu},
  							{"o",	PY_mb_mo},
  							{"ou",	PY_mb_mou},
  							{"u",	PY_mb_mu}};
const typPinYin PY_index_n[]={
  							{"a",	PY_mb_na},
  							{"ai",	PY_mb_nai},
  							{"an",	PY_mb_nan},
  							{"ang",	PY_mb_nang},
  							{"ao",	PY_mb_nao},
  							{"e",	PY_mb_ne},
  							{"ei",	PY_mb_nei},
  							{"en",	PY_mb_nen},
  							{"eng",	PY_mb_neng},
  							{"i",	PY_mb_ni},
  							{"ian",	PY_mb_nian},
  							{"iang",PY_mb_niang},
  							{"iao",	PY_mb_niao},
  							{"ie",	PY_mb_nie},
  							{"in",	PY_mb_nin},
  							{"ing",	PY_mb_ning},
  							{"iu",	PY_mb_niu},
  							{"ong",	PY_mb_nong},
  							{"u",	PY_mb_nu},
  							{"uan",	PY_mb_nuan},
  							{"ue",	PY_mb_nue},
  							{"uo",	PY_mb_nuo},
  							{"v",	PY_mb_nv}};
const typPinYin PY_index_o[]={
  							{"",	PY_mb_o},
  							{"u",	PY_mb_ou}};
const typPinYin PY_index_p[]={
  							{"a",	PY_mb_pa},
  							{"ai",	PY_mb_pai},
  							{"an",	PY_mb_pan},
  							{"ang",	PY_mb_pang},
  							{"ao",	PY_mb_pao},
  							{"ei",	PY_mb_pei},
  							{"en",	PY_mb_pen},
  							{"eng",	PY_mb_peng},
  							{"i",	PY_mb_pi},
  							{"ian",	PY_mb_pian},
  							{"iao",	PY_mb_piao},
  							{"ie",	PY_mb_pie},
  							{"in",	PY_mb_pin},
  							{"ing",	PY_mb_ping},
  							{"o",	PY_mb_po},
  							{"ou",	PY_mb_pou},
  							{"u",	PY_mb_pu}};
const typPinYin PY_index_q[]={
  							{"i",	PY_mb_qi},
  							{"ia",	PY_mb_qia},
  							{"ian",	PY_mb_qian},
  							{"iang",PY_mb_qiang},
  							{"iao",	PY_mb_qiao},
  							{"ie",	PY_mb_qie},
  							{"in",	PY_mb_qin},
  							{"ing",	PY_mb_qing},
  							{"iong",PY_mb_qiong},
  							{"iu",	PY_mb_qiu},
  							{"u",	PY_mb_qu},
  							{"uan",	PY_mb_quan},
  							{"ue",	PY_mb_que},
  							{"un",	PY_mb_qun}};
const typPinYin PY_index_r[]={
  							{"an",	PY_mb_ran},
  							{"ang",	PY_mb_rang},
  							{"ao",	PY_mb_rao},
  							{"e",	PY_mb_re},
  							{"en",	PY_mb_ren},
  							{"eng",	PY_mb_reng},
  							{"i",	PY_mb_ri},
  							{"ong",	PY_mb_rong},
  							{"ou",	PY_mb_rou},
  							{"u",	PY_mb_ru},
  							{"uan",	PY_mb_ruan},
  							{"ui",	PY_mb_rui},
  							{"un",	PY_mb_run},
  							{"uo",	PY_mb_ruo}};
const typPinYin PY_index_s[]={
  							{"a",	PY_mb_sa},
  							{"ai",	PY_mb_sai},
  							{"an",	PY_mb_san},
  							{"ang",	PY_mb_sang},
  							{"ao",	PY_mb_sao},
  							{"e",	PY_mb_se},
  							{"en",	PY_mb_sen},
  							{"eng",	PY_mb_seng},
  							{"ha",	PY_mb_sha},
  							{"hai",	PY_mb_shai},
  							{"han",	PY_mb_shan},
  							{"hang",PY_mb_shang},
  							{"hao",	PY_mb_shao},
  							{"he",	PY_mb_she},
  							{"hen",	PY_mb_shen},
  							{"heng",PY_mb_sheng},
  							{"hi",	PY_mb_shi},
  							{"hou",	PY_mb_shou},
  							{"hu",	PY_mb_shu},
  							{"hua",	PY_mb_shua},
  							{"huai",PY_mb_shuai},
  							{"huan",PY_mb_shuan},
  							{"huang",PY_mb_shuang},
  							{"hui",	PY_mb_shui},
  							{"hun",	PY_mb_shun},
  							{"huo",	PY_mb_shuo},
  							{"i",	PY_mb_si},
  							{"ong",	PY_mb_song},
  							{"ou",	PY_mb_sou},
  							{"u",	PY_mb_su},
  							{"uan",	PY_mb_suan},
  							{"ui",	PY_mb_sui},
  							{"un",	PY_mb_sun},
  							{"uo",	PY_mb_suo}};
const typPinYin PY_index_t[]={
  							{"a",	PY_mb_ta},
  							{"ai",	PY_mb_tai},
  							{"an",	PY_mb_tan},
  							{"ang",	PY_mb_tang},
  							{"ao",	PY_mb_tao},
  							{"e",	PY_mb_te},
  							{"eng",	PY_mb_teng},
  							{"i",	PY_mb_ti},//
  							{"ian",	PY_mb_tian},
  							{"iao",	PY_mb_tiao},
  							{"ie",	PY_mb_tie},
  							{"ing",	PY_mb_ting},
  							{"ong",	PY_mb_tong},
  							{"ou",	PY_mb_tou},
  							{"u",	PY_mb_tu},
  							{"uan",	PY_mb_tuan},
  							{"ui",	PY_mb_tui},
  							{"un",	PY_mb_tun},
  							{"uo",	PY_mb_tuo}};
const typPinYin PY_index_w[]={
  							{"a",	PY_mb_wa},
  							{"ai",	PY_mb_wai},
  							{"an",	PY_mb_wan},
  							{"ang",	PY_mb_wang},
  							{"ei",	PY_mb_wei},
  							{"en",	PY_mb_wen},
  							{"eng",	PY_mb_weng},
  							{"o",	PY_mb_wo},
  							{"u",	PY_mb_wu}};
const typPinYin PY_index_x[]={
  							{"i",	PY_mb_xi},
  							{"ia",	PY_mb_xia},
  							{"ian",	PY_mb_xian},
  							{"iang",PY_mb_xiang},
  							{"iao",	PY_mb_xiao},
  							{"ie",	PY_mb_xie},
  							{"in",	PY_mb_xin},
  							{"ing",	PY_mb_xing},
  							{"iong",PY_mb_xiong},
  							{"iu",	PY_mb_xiu},
  							{"u",	PY_mb_xu},
  							{"uan",	PY_mb_xuan},
  							{"ue",	PY_mb_xue},
  							{"un",	PY_mb_xun}};
const typPinYin PY_index_y[]={
  							{"a",	PY_mb_ya},
  							{"an",	PY_mb_yan},
  							{"ang",	PY_mb_yang},
  							{"ao",	PY_mb_yao},
  							{"e",	PY_mb_ye},
  							{"i",	PY_mb_yi},
  							{"in",	PY_mb_yin},
  							{"ing",	PY_mb_ying},
  							{"o",	PY_mb_yo},
  							{"ong",	PY_mb_yong},
  							{"ou",	PY_mb_you},
  							{"u",	PY_mb_yu},
  							{"uan",	PY_mb_yuan},
  							{"ue",	PY_mb_yue},
  							{"un",	PY_mb_yun}};
const typPinYin PY_index_z[]={
  							{"a",	PY_mb_za},
  							{"ai",	PY_mb_zai},
  							{"an",	PY_mb_zan},
  							{"ang",	PY_mb_zang},
  							{"ao",	PY_mb_zao},
  							{"e",	PY_mb_ze},
  							{"ei",	PY_mb_zei},
  							{"en",	PY_mb_zen},
  							{"eng",	PY_mb_zeng},
  							{"ha",	PY_mb_zha},
  							{"hai",	PY_mb_zhai},
  							{"han",	PY_mb_zhan},
  							{"hang",PY_mb_zhang},
  							{"hao",	PY_mb_zhao},
  							{"he",	PY_mb_zhe},
  							{"hen",	PY_mb_zhen},
  							{"heng",PY_mb_zheng},
  							{"hi",	PY_mb_zhi},
  							{"hong",PY_mb_zhong},
  							{"hou",	PY_mb_zhou},
  							{"hu",	PY_mb_zhu},
  							{"hua",	PY_mb_zhua},
  							{"huai",PY_mb_zhuai},
  							{"huan",PY_mb_zhuan},
  							{"huang",PY_mb_zhuang},
  							{"hui",	PY_mb_zhui},
  							{"hun",	PY_mb_zhun},
  							{"huo",	PY_mb_zhuo},
  							{"i",	PY_mb_zi},
  							{"ong",	PY_mb_zong},
  							{"ou",	PY_mb_zou},
  							{"u",	PY_mb_zu},
  							{"uan",	PY_mb_zuan},
  							{"ui",	PY_mb_zui},
  							{"un",	PY_mb_zun},
  							{"uo",	PY_mb_zuo}};
const typPinYin PY_index_end[]={" ",PY_mb_none};



//================================================================================================



//================================================================================================

//"========================================±äÁ¿ÉùÃ÷========================================"
//unsigned int py_mb_begin=(unsigned int)&PY_mb_a[0];
const typPinYin*  PY_index_pointer[27]={	PY_index_a,PY_index_b,PY_index_c,PY_index_d,PY_index_e,PY_index_f,PY_index_g,PY_index_h,
											PY_index_j,PY_index_j,PY_index_k,PY_index_l,PY_index_m,PY_index_n,PY_index_o,PY_index_p,
											PY_index_q,PY_index_r,PY_index_s,PY_index_t,PY_index_w,PY_index_w,PY_index_w,PY_index_x,
											PY_index_y,PY_index_z,PY_index_end};


//"========================================Ö÷³ÌĞòÌå========================================"

const char * py_ime(unsigned char input_py[])
{
	const typPinYin* p1;
	const typPinYin* p2;
	const char* ret;
	uint8_t* pInput = input_py;
	
	while(*pInput != '\0')
	{
		if(*pInput>=0x80)
			pInput+=2;
		else
			break;
	}
	
	if (pInput[0]==0) return(0);																				//"Èç¹ûÊäÈë¿Õ×Ö·û·µ»Ø0"
	if(pInput[0]<'a' || pInput[0]>'z') return (0);
	if (pInput[0]=='i') return(0);
	if (pInput[0]=='u') return(0);
	if (pInput[0]=='v') return(0);
//	if (input_py_val[0]=='i') return(0);
//	if (input_py_val[0]=='u') return(0);
//	if (input_py_val[0]=='v') return(0);
	
	
	p1=PY_index_pointer[pInput[0]-0x61];															//"¼ÆËãÈë¿ÚÊ÷¸ù"
	p2=PY_index_pointer[pInput[0]-0x60];																//"ÉèÖÃÖ¸Õë½çÏŞ"
	while(p1<p2)
	{
		if(strcmp(p1->YunMu,(const char*)&pInput[1])== 0 )
		{
//			ret = (const char*)(p1->offset_high *256 + p1->offset_low +py_mb_begin);
			ret = (const char*)(p1->MB);
			return ret;
		}
		else
		{
			p1++;
		}
	}
	
	return 0;
}




