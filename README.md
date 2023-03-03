# libxml2++
封装了一下libxml2

libxml2不再介绍，使用的很多，我今天只是分享一下我在使用过程中遇到的一些问题：

1. 解析时候设置合理的选项可以避免在XML解析过程中，将空白部分文本解析为节点；
```c++
int opt = XML_PARSE_RECOVER | XML_PARSE_NOBLANKS | XML_PARSE_NOERROR;
Xml2Doc doc = Xml2Doc::tryParseFile(fileName, "", opt);
```

2. html文件大多都有错误，所以在解析时候需要忽略错误以及提示消息：
```
XML_PARSE_RECOVER 
XML_PARSE_NOERROR
XML_PARSE_NOWARNING
```
3. 还有页面不符合XML规范，节点没有闭合，造成XPATH搜不到任何东西，主要是解析树时候名字丢失了，但是通过自己手写深度递归查找还是可以的，
比如下面一段

```html
<div class="SoundBox oh last" data-id="294360">
                <div class="SoundContent">
                    <div class="buttons"><a target="_blank" href="https://www.yespik.com/download-sound_294360_1.html"
                            class="downBtn">立即下载</a><a href="javascript:;" onclick="listFav(294360,this,21)"
                            class="fav favBtn" fav-id="294360" action="add"></a> </div>
                    <div class="SoundTitle"> <a target="_blank"
                            href="https://www.yespik.com/show-sound_294360.html">震撼大气的年会颁奖片头上场背景音乐</a> </div>
                    <div class="SoundPlayer">
                        <div class="SoundDiskBox pr fl SoundDiskBox114 StartPlay" data-id="294360"
                            data-mp3="preview/sound/00/29/43/515ppt-S294360-6C1F5108.mp3" data-width="164">
                            <div class="SoundPlayerBg"></div>
                            <!--<div class="SoundPlayerBtn pa opacity-8 SoundPlayerBtn114"></div>-->
                        </div> 
                         <audio preload="none" data-time="63">
                            <source
                                src="//img-bsy2.yespik.com/sound/00/29/43/60/294360_a4f43b70e64c10553ba7fb8451dcd269.mp3"
                                type="audio/mpeg">
                        </audio>
                        <div class="DurationBox">
                            <div class="SoundStartTime fl star-time">00:00</div>
                            <div class="SoundWave fl pr time-bar"> <span class="progressBar"></span> <i
                                    class="move-color"></i>
                                <p class="timetip"></p>
                            </div>
                            <div class="SoundEndTime fl end-time">01:03</div>
                        </div>
                    </div>
                </div>
            </div>
```
可以使用自己手写的查找函数来实现提取音乐信息
```c++
// https://www.yespik.com/sound/0-5_0_0_0-0-default/p_1/
void testDeepFind()
{
    string fileName = "音效素材下载-音效大全-配乐.htm";
    int opt = XML_PARSE_RECOVER | XML_PARSE_NOBLANKS | XML_PARSE_NOERROR;
    Xml2Doc doc = Xml2Doc::tryParseFile(fileName, "", opt);
    if (doc.isNull())
        return;
 
    // 根据属性对，递归找到外层
    Xml2NodeArray vec = doc.getByName("div", "class", "SoundContent");
    for (auto& it : vec)
    {
        std::cout << it.getName() << "[ ";
        
        // 从此节点再次找source节点
        Xml2NodeArray srcList = it.getDeepElementsByName("source", "", "");
        if (srcList.size() > 0)
        {
            printAttrs(srcList[0]);
        }
        
        // 从此节点找带有  SoundTitle 属性值的节点，从里面找到文本信息
        Xml2NodeArray titleList = it.getDeepElementsByName("div", "class", "SoundTitle");
        if (titleList.size() > 0)
        {
            Xml2NodeArray txtList = titleList[0].getChildrenByName("a");
            if (txtList.size() > 0)
            {
                string val = StringEncoding::utf8ToAnsi(txtList[0].getValue());
                cout << val;
            }
                
        }
        std::cout << "] \n ";
    }
}
```
输出的结果如下：
```
div[ src: //img-bsy2.yespik.com/sound/00/29/33/62/293362_0563242c78713029ba085fdcc42e7f99.mp3, type: audio/mpeg, 励志正
能量影视钢琴配乐]
 div[ src: //img-bsy2.yespik.com/sound/00/29/33/67/293367_1f1c677b344c570c958d1af4cc477278.mp3, type: audio/mpeg, 正能量积极向上宣传片背景音乐]
 div[ src: //img-bsy2.yespik.com/sound/00/29/52/34/295234_1708512ee79d90f3174048532136a468.mp3, type: audio/mpeg, 抒情正能量背景音乐]
 div[ src: //img-bsy2.yespik.com/sound/00/29/82/02/298202_0477ab77254c1a26f5178aae644cb37f.mp3, type: audio/mpeg, 明亮闪光的正能量素材音]
 div[ src: //img-bsy2.yespik.com/sound/00/29/40/53/294053_583578e216f8f4a9fe51aefff3c5fbd0.mp3, type: audio/mpeg, 现代科技感广告背景音乐]
 div[ src: //img-bsy2.yespik.com/sound/00/29/54/27/295427_c2dd581edca576425de4a17fb6b6f36c.mp3, type: audio/mpeg, 励志正能量的企业背景素材音乐]
 div[ src: //img-bsy2.yespik.com/sound/00/29/54/28/295428_3c4d67329cceea76eecb5669b1ad926b.mp3, type: audio/mpeg, 励志正能量的企业宣传音频]
 div[ src: //img-bsy2.yespik.com/sound/00/30/29/43/302943_ed93ce5dce1a8493a51b29cff63b6b49.mp3, type: audio/mpeg, 快手抖音积极正能量小清新短视频配乐]
 div[ src: //img-bsy2.yespik.com/sound/00/31/55/85/315585_2d9ee7023f70592fd61a068f30815f67.mp3, type: audio/mpeg, 正能量励志小节奏感配乐]
 div[ src: //img-bsy2.yespik.com/sound/00/31/27/28/312728_b576a2d82f2be11edb92fb73e095b517.mp3, type: audio/mpeg, 正能量团队精神背景配乐]
 div[ src: //img-bsy2.yespik.com/sound/00/29/34/25/293425_0bf70f788e1247f022c2ef85893d5ae3.mp3, type: audio/mpeg, 点燃你心里的火焰浪漫配乐]
 div[ src: //img-bsy2.yespik.com/sound/00/31/00/12/310012_f695b25ec047424f4bf3ae869a7fb804.mp3, type: audio/mpeg, 安静轻松愉快背景音乐]
 div[ src: //img-bsy2.yespik.com/sound/00/30/84/87/308487_23472e91cbb0b9ec205d363bbf40678b.mp3, type: audio/mpeg, 春日花香灵动旋律活泼律动]
 div[ src: //img-bsy2.yespik.com/sound/00/30/81/42/308142_ebdeb35b1d67ba0fc2bbe4cbe0ecdeaf.mp3, type: audio/mpeg, 激昂的钢琴曲]
 div[ src: //img-bsy2.yespik.com/sound/00/29/34/54/293454_f92d4f1b78ff82197d6cb9e4863f8bc9.mp3, type: audio/mpeg, 正能量积极向上宣传片背景音乐]
 div[ src: //img-bsy2.yespik.com/sound/00/29/34/29/293429_5ecac3c3e1bd2eb9e7d9e379535c81b8.mp3, type: audio/mpeg, 励志正能量影视配乐]
 div[ src: //img-bsy2.yespik.com/sound/00/31/01/17/310117_d677a1a0132691d01f2589f64a08431a.mp3, type: audio/mpeg, 励志正能量会议背景音乐]
 div[ src: //img-bsy2.yespik.com/sound/00/29/54/23/295423_ae16db738f3499de8e133cd067c5c042.mp3, type: audio/mpeg, 积极正能量的电视栏目包装背景音乐]
 div[ src: //img-bsy2.yespik.com/sound/00/31/01/12/310112_e5d4a01b9621257de9518a33b4f92412.mp3, type: audio/mpeg, 正能量ppt背景音乐]
 div[ src: //img-bsy2.yespik.com/sound/00/31/00/14/310014_ebf1c8ff9802eec995be463b0bce8da8.mp3, type: audio/mpeg, 温暖安静唯美背景音乐]
 div[ src: //img-bsy2.yespik.com/sound/00/29/53/18/295318_6a7452cdd1995165d014206701d428c9.mp3, type: audio/mpeg, 积极正能量的电视栏目包装]
 div[ src: //img-bsy2.yespik.com/sound/00/29/34/22/293422_7633a284d568a57a87e79d7975584fbe.mp3, type: audio/mpeg, 励志正能量影视钢琴配乐]
 div[ src: //img-bsy2.yespik.com/sound/00/30/43/52/304352_316d5b7566b1ff959cbb03630005ea7c.mp3, type: audio/mpeg, 快节奏的展现力量的音乐]
 div[ src: //img-bsy2.yespik.com/sound/00/31/57/18/315718_b703e16b74079d2c8a1f31cbde51e02f.mp3, type: audio/mpeg, 进取正能量金融行业发布会配乐]
 div[ src: //img-bsy2.yespik.com/sound/00/30/36/12/303612_099a56a3ac64d490a79259dcb656c9a5.mp3, type: audio/mpeg, 辉煌的时刻，激励人心的正能量素材音]
 div[ src: //img-bsy2.yespik.com/sound/00/29/49/69/294969_f50f2293d0a03abe250cad79fa639fcf.mp3, type: audio/mpeg, 充满希望的比赛背景音]
 div[ src: //img-bsy2.yespik.com/sound/00/31/04/03/310403_6c1884b6f5bb528519e8fb93be8e9217.mp3, type: audio/mpeg, 休闲轻松的高山流水曲目背景音乐]
 div[ src: //img-bsy2.yespik.com/sound/00/32/33/85/323385_487c09280ebf57f5168e01ad9de053f5.mp3, type: audio/mpeg, 森林活泼正能量阳光积极背景配乐]
 div[ src: //img-bsy2.yespik.com/sound/00/29/39/32/293932_afec8b14f8a4120c254a2f92fef9ed20.mp3, type: audio/mpeg, 震撼热血的年会背景配乐]
 div[ src: //img-bsy2.yespik.com/sound/00/30/07/23/300723_fc398e6635ded40282ae812a509f1984.mp3, type: audio/mpeg, 励志正能量电子配乐]
 div[ src: //img-bsy2.yespik.com/sound/00/29/42/98/294298_e77209651a7e213f9c3df9a5d45bd372.mp3, type: audio/mpeg, 中国风企业正能量背景音配乐配乐]
 div[ src: //img-bsy2.yespik.com/sound/00/29/39/33/293933_edfa27e04022bb4b72eb07edce7eeff5.mp3, type: audio/mpeg, 振奋激昂的年会背景音乐]
 div[ src: //img-bsy2.yespik.com/sound/00/30/35/62/303562_30649acd2bf6da32aa0c8d9b95bda353.mp3, type: audio/mpeg, 澎湃激情城市宣传片背景音乐]
 div[ src: //img-bsy2.yespik.com/sound/00/29/77/07/297707_59a900a34c2400d6e3c942888850756b.mp3, type: audio/mpeg, 安静温暖阳光钢琴安眠曲]
 div[ src: //img-bsy2.yespik.com/sound/00/29/96/93/299693_158896b2230fbd52ef32e33e91b76eba.mp3, type: audio/mpeg, 清新欢乐的产品宣传片配乐]
 div[ src: //img-bsy2.yespik.com/sound/00/32/13/87/321387_c2dd58edb904ed08551bd3ab6a21aaad.mp3, type: audio/mpeg, 节奏阴沉昏暗钢琴曲配乐]
 div[ src: //img-bsy2.yespik.com/sound/00/29/93/92/299392_e4923a59bdccd1bacad9acfeb2901a80.mp3, type: audio/mpeg, 回想过去钢琴曲]
 div[ src: //img-bsy2.yespik.com/sound/00/31/47/30/314730_1cac9a3b48522048e660ca1e8e6d11fe.mp3, type: audio/mpeg, 正能量加油前进背景配乐]
 div[ src: //img-bsy2.yespik.com/sound/00/29/58/98/295898_43203ebfc2bddcfb52fdc9135997c967.mp3, type: audio/mpeg, 紫色云彩弦乐配乐]
```
