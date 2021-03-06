/*
 * sound\soc\sunxi\sunxi_snddaudio0.c
 * (C) Copyright 2014-2016
 * Reuuimlla Technology Co., Ltd. <www.reuuimllatech.com>
 * huangxin <huangxin@Reuuimllatech.com>
 *
 * some simple description for this code
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 */

#include <linux/module.h>
#include <linux/clk.h>
#include <linux/mutex.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>
#include <sound/soc-dapm.h>
#include <linux/io.h>
#include <linux/of.h>
#include "sunxi_tdm_utils.h"
#include "codec-utils.h"

static int sunxi_snddaudio0_hw_params(struct snd_pcm_substream *substream,
					struct snd_pcm_hw_params *params)
{
	int ret  = 0;
	u32 freq = 22579200;

	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	unsigned long sample_rate = params_rate(params);
	struct sunxi_tdm_info  *sunxi_daudio = snd_soc_dai_get_drvdata(cpu_dai);

	switch (sample_rate) {
		case 8000:
		case 16000:
		case 32000:
		case 64000:
		case 128000:
		case 12000:
		case 24000:
		case 48000:
		case 96000:
		case 192000:
			freq = 24576000;
			break;
	}

	pr_info("[daudio0]sample rate %ld\n", sample_rate);

	/*set system clock source freq and set the mode as daudio or pcm*/
	ret = snd_soc_dai_set_sysclk(cpu_dai, 0 , freq, 0);
	if (ret < 0) {
	    	pr_warn("[daudio0]set sysclk failed %d\n", ret);
		return ret;
	}

	/*set system clock source freq and set the mode as daudio or pcm*/
	ret = snd_soc_dai_set_sysclk(codec_dai, 0 , 12288000, 0);
	if (ret < 0) {
		pr_warn("[daudio0],the codec_dai set set_sysclk failed.\n");
	}

	ret = snd_soc_dai_set_fmt(codec_dai,
				  SND_SOC_DAIFMT_I2S |
				  SND_SOC_DAIFMT_NB_NF |
				  (sunxi_daudio->daudio_master<<12));
	if (ret < 0) {
		pr_warn("[daudio0],the codec_dai set set_fmt failed.\n");
	}
	/*
	* codec clk & FRM master. AP as slave
	*/

	pr_info("[daudio0]cpu_dai: fmt %x, inversion %x, master %x\n",
		sunxi_daudio->audio_format,
		(sunxi_daudio->signal_inversion <<8),
		(sunxi_daudio->daudio_master <<12));

	ret = snd_soc_dai_set_fmt(cpu_dai, (sunxi_daudio->audio_format| (sunxi_daudio->signal_inversion <<8) | (sunxi_daudio->daudio_master <<12)));
	if (ret < 0) {
	    	pr_warn("[daudio0]set fmt cpu_dai failed %d\n", ret);
		return ret;
	}

	ret = snd_soc_dai_set_clkdiv(cpu_dai, 0, sample_rate);
	if (ret < 0) {
	    	pr_warn("[daudio0]set cpu_dai clkdiv failed %d\n", ret);
		return ret;
	}
	ret = snd_soc_dai_set_clkdiv(codec_dai, 0, sample_rate);
	if (ret < 0) {
		pr_warn("[daudio0],the codec_dai set set_clkdiv failed.\n");
	}

	return 0;
}

/*
 * Card initialization
 */
static int sunxi_daudio_init(struct snd_soc_pcm_runtime *rtd)
{
	return 0;
}

static struct snd_soc_ops sunxi_snddaudio_ops = {
	.hw_params 		= sunxi_snddaudio0_hw_params,
};

static struct snd_soc_dai_link sunxi_snddaudio_dai_link = {
	.name 			= "hifi",
	.stream_name 	= "SUNXI-TDM0",
	.cpu_dai_name 	= "sunxi-daudio",
	.init 			= sunxi_daudio_init,
	.platform_name 	= "sunxi-daudio",
	.ops 			= &sunxi_snddaudio_ops,
};

static struct snd_soc_card snd_soc_sunxi_snddaudio = {
	.name 		= "snddaudio0",
	.owner 		= THIS_MODULE,
	.dai_link 	= &sunxi_snddaudio_dai_link,
	.num_links 	= 1,
};

static int  sunxi_snddaudio0_dev_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct device_node *np = pdev->dev.of_node;
	struct snd_soc_card *card = &snd_soc_sunxi_snddaudio;

	dev_info(&pdev->dev, "probe start\n");

	card->dev = &pdev->dev;
	sunxi_snddaudio_dai_link.cpu_dai_name = NULL;
	sunxi_snddaudio_dai_link.cpu_of_node = of_parse_phandle(np,
				"sunxi,daudio0-controller", 0);
	if (!sunxi_snddaudio_dai_link.cpu_of_node) {
		dev_err(&pdev->dev,
			"Property 'sunxi,daudio0-controller' missing or invalid\n");
			ret = -EINVAL;
	}
	sunxi_snddaudio_dai_link.platform_name = NULL;
	sunxi_snddaudio_dai_link.platform_of_node = sunxi_snddaudio_dai_link.cpu_of_node;

	sunxi_snddaudio_dai_link.codec_of_node = of_parse_phandle(np,"sunxi,audio-codec", 0);
	if (of_property_read_string_index(np, "sunxi,audio-dai-name", 0,
					  &sunxi_snddaudio_dai_link.codec_dai_name) < 0)
	{
	    sunxi_snddaudio_dai_link.codec_dai_name = NULL;
	}

	if (sunxi_snddaudio_dai_link.codec_dai_name == NULL &&
	    sunxi_snddaudio_dai_link.codec_name == NULL) {
	    		sunxi_snddaudio_dai_link.codec_of_node = NULL;
			codec_utils_probe(pdev);
			sunxi_snddaudio_dai_link.codec_dai_name = pdev->name;
			sunxi_snddaudio_dai_link.codec_name 	= pdev->name;
	}
	if (sunxi_snddaudio_dai_link.codec_name) {
	    dev_info(&pdev->dev, "codec name %s\n", sunxi_snddaudio_dai_link.codec_name);
	}
	if (sunxi_snddaudio_dai_link.codec_dai_name) {
	    dev_info(&pdev->dev, "codec dai name %s\n", sunxi_snddaudio_dai_link.codec_dai_name);
	}

	ret = snd_soc_register_card(card);
	if (ret) {
		dev_err(&pdev->dev, "snd_soc_register_card() failed: %d\n", ret);
	}

	dev_info(&pdev->dev, "probe %d\n", ret);

	return ret;
}

static int  sunxi_snddaudio0_dev_remove(struct platform_device *pdev)
{
	struct snd_soc_card *card = platform_get_drvdata(pdev);
	snd_soc_unregister_card(card);
	return 0;
}

static const struct of_device_id sunxi_daudio0_of_match[] = {
	{ .compatible = "allwinner,sunxi-daudio0-machine", },
	{},
};

/*method relating*/
static struct platform_driver sunxi_daudio_driver = {
	.driver = {
		.name = "snddaudio0",
		.owner = THIS_MODULE,
		.pm = &snd_soc_pm_ops,
		.of_match_table = sunxi_daudio0_of_match,
	},
	.probe = sunxi_snddaudio0_dev_probe,
	.remove= sunxi_snddaudio0_dev_remove,
};

static int __init sunxi_snddaudio0_init(void)
{
	int err = 0;
	if ((err = platform_driver_register(&sunxi_daudio_driver)) < 0)
		return err;
	return 0;
}
module_init(sunxi_snddaudio0_init);

static void __exit sunxi_snddaudio0_exit(void)
{
	platform_driver_unregister(&sunxi_daudio_driver);
}
module_exit(sunxi_snddaudio0_exit);
MODULE_AUTHOR("huangxin");
MODULE_DESCRIPTION("SUNXI_snddaudio ALSA SoC audio driver");
MODULE_LICENSE("GPL");
