// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * PythonのArgumentParserのようにコマンド引数をパースするクラス
 *
 * 使用例

FArgParser ArgParser;

// 引数の登録
ArgParser.AddArg(TEXT("-pos"), true, FArgParser::EType::Vector);
ArgParser.AddArg(TEXT("-intValue"), false, FArgParser::EType::Integer);

// 文字列をパースして引数の値を取得する
const FString Command = TEXT("SampleCommand -pos \"V(X=10.00, Y=20.00, Z=30.00)\" -intValue -1234");
FVector VectorValue;
int32 IntValue;
const bool bIsValid = ArgParser.Parse(Command) &&
	ArgParser.GetValue(TEXT("-pos"), VectorValue) &&
	ArgParser.GetValue(TEXT("-intValue"), IntValue);

if (bIsValid)
{
	UE_LOG(LogTemp, Log, TEXT("VectorValue:%s IntValue:%d"), *VectorValue.ToString(), IntValue);
}

 */
class FArgParser final
{
public:

	/**
	 * @brief 型情報
	 *		　引数の型チェックで使用します
	 */
	enum class EType
	{
		// なし。型チェック実行されません。
		None,
		// 整数値
		Integer,
		// 浮動小数値
		Float,
		// 真偽値
		Bool,
		// FVector値
		Vector
	};

	/**
	 * @brief 引数情報追加
	 * @param ArgName 引数名
	 * @param bRequired 必須引数か
	 * @param ValidateType 引数の値に求める型
	 */
	void AddArg(const FString& ArgName, bool bRequired = false, EType ValidateType = EType::None);

	/**
	 * @brief 引数パース
	 *		　 AddArgで追加した引数情報をもとにパースを行います。
	 *		　 必須引数がなかったり、型が一致しない場合はパースに失敗します。
	 * @param Command パース対象コマンド文字列
	 * @return パースに成功した場合trueを返します
	 */
	bool Parse(const FString& Command);


	/**
	 * @brief パースした情報をリセットします
	 */
	void Reset();

	// パースした値取得。取得に失敗する場合falseを返します。
	bool GetValue(const FString& ArgName, int8& Value) const;
	bool GetValue(const FString& ArgName, int16& Value) const;
	bool GetValue(const FString& ArgName, int32& Value) const;
	bool GetValue(const FString& ArgName, int64& Value) const;
	bool GetValue(const FString& ArgName, float& Value) const;
	bool GetValue(const FString& ArgName, FString& Value) const;
	bool GetValue(const FString& ArgName, bool& Value) const;
	bool GetValue(const FString& ArgName, FVector& Value) const;

private:

	/**
	 * @brief 引数情報
	 *		　引数をパースし、パースした値を持つ
	 *		　パース時に正当性チェックを行う
	 */
	class FArgInfo final
	{
	public:
		/**
		 * @param ValidateType 検証する型タイプ
		 * @param bRequired 必須引数か
		 */
		FArgInfo(EType ValidateType, bool bRequired);

		/**
		 * @brief 引数をパースします
		 * @param ArgName 引数名
		 * @param Command パース対象のコマンド文字列
		 * @return 
		 */
		bool Parse(const FString& ArgName, const FString& Command);

		/**
		 * @brief パース処理実行済みかどうか
		 */
		bool IsParsed() const;

		/**
		 * @brief パース情報をリセットする
		 */
		void Reset();

		/**
		 * @brief パース済みの文字列を取得する
		 */
		const FString& GetParsedValue() const;

		/**
		 * @brief 検証型取得
		 */
		EType GetValidateType() const;

	private:
		EType ValidateType;
		bool bRequired;
		TOptional<FString> ParsedValue;

		/**
		 * @brief 使用する正規表現パターン
		 */
		class FRegexData final
		{
		public:
			FRegexData() :
				QuotationValuePattern(TEXT("^\"([^\"]+)\"")),
				NoQuotationPattern(TEXT("^(\\S+)")),
				FloatPattern(TEXT("^([+-]?\\d+\\.?[\\d]*)$")),
				IntegerPattern(TEXT("^([+-]?\\d+)$"))
			{
			}

			// 引数の値が入っている文字列から""でくくられている値を抽出するパターン
			const FRegexPattern QuotationValuePattern;

			// 引数の値が入っている文字列から空白までの間の値を抽出するパターン
			const FRegexPattern NoQuotationPattern;

			// 浮動小数マッチパターン
			const FRegexPattern FloatPattern;

			// 整数値マッチパターン
			const FRegexPattern IntegerPattern;
		};

		// static変数でFRegexPatternは初期化できないため。必要になったタイミングでnewする。
		static TSharedPtr<FRegexData> RegexData;

		/**
		 * @brief 型タイプ検証
		 * @param ArgName 引数名
		 * @param ArgValue 引数の値
		 * @param validateType 検証する型
		 * @return 検証の結果有効であればtrueを返します。
		 */
		static bool ValidateArgType(const FString& ArgName, const FString& ArgValue, EType validateType);
	};


	/**
	 * @brief 有効な引数かどうか
	 * @param ArgName 引数名
	 * @param RequiredType 引数に求める型
	 * @return 有効な場合trueを返します
	 */
	bool IsValidArg(const FString& ArgName, EType RequiredType) const;

	template <typename T>
	bool GetValueInteger(const FString& ArgName, T& Value) const
	{
		if (IsValidArg(ArgName, EType::Integer))
		{
			Value = FCString::Atoi(*ArgInfos[ArgName].GetParsedValue());
			return true;
		}
		else
		{
			return false;
		}
	}

	TMap<FString, FArgInfo> ArgInfos;
	TOptional<bool> bIsValid;
};
