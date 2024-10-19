# TenshokuMaou

某素材。  

## 外部実行ファイル

ファイル展開のため、[basis_universal](https://github.com/BinomialLLC/basis_universal/releases/tag/v1_50_0_2)の`basisu.exe`を利用します。  
但し、RGBA形式の出力ファイル名を指定するために以下の変更を加えてビルドする必要があります。
1.  元々のファイル出力部分を削除
2.  コマンドライン引数からファイル名を生成するようコードを追加

### 変更箇所
``` cpp
else /* v1.50.0では2433行目*/
{
	image u;
	if (!gi[level_index].unpack(u))
	{
		printf("Warning: Failed unpacking GPU texture data (%u %u %u). Unpacking as much as possible.\n", format_iter, image_index, level_index);
		total_unpack_warnings++;
	}

	bool write_png = true;

	/* 元々の出力コードはコメント化 */

	if (basis_transcoder_format_has_alpha(transcoder_tex_fmt) && (!opts.m_ktx_only) && (write_png))
	{
		/* 追加コード */
		std::string output_filepath;
		string_combine_path(output_filepath, opts.m_output_path.c_str(), opts.m_output_filename.c_str());
		if (!save_png(output_filepath, u))
		{
			error_printf("Failed writing to PNG file \"%s\"\n", output_filepath.c_str());
			return false;
		}
		printf("Wrote PNG file \"%s\"\n", output_filepath.c_str());
	}

```
