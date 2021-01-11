// Fill out your copyright notice in the Description page of Project Settings.


#include "ArgParser.h"


void FArgParser::AddArg(const FString& ArgName, bool bRequired, EType ValidateType)
{
	if (!ensureAlwaysMsgf(!bIsValid.IsSet(), TEXT("すでにパース済みです")))
	{
		return;
	}

	if (!ensureAlwaysMsgf(!ArgInfos.Contains(ArgName), TEXT("すでに登録済みの引数名です: %s"), *ArgName))
	{
		return;
	}

	ArgInfos.Add(ArgName, FArgInfo{ValidateType, bRequired});
}

bool FArgParser::Parse(const FString& Command)
{
	bIsValid.Reset();

	bool bSuccess = true;
	for (auto& ArgElem : ArgInfos)
	{
		ArgElem.Value.Reset();

		if (!ArgElem.Value.Parse(ArgElem.Key, Command))
		{
			bSuccess = false;
			break;
		}
	}

	if (!bSuccess)
	{
		UE_LOG(LogTemp, Error, TEXT("コマンド %s のパースに失敗しました"), *Command);
	}

	bIsValid = bSuccess;
	return bIsValid.GetValue();
}

void FArgParser::Reset()
{
	bIsValid.Reset();
	ArgInfos.Reset();
}

bool FArgParser::IsValidArg(const FString& ArgName, EType RequiredType) const
{
	if (bIsValid.IsSet() && bIsValid.GetValue())
	{
		const bool bIsValidArg = ensureAlwaysMsgf(ArgInfos.Contains(ArgName), TEXT("引数 %s はパース対象ではありません"), *ArgName) &&
			ensureAlwaysMsgf(ArgInfos[ArgName].IsParsed(), TEXT("引数 %s がパースされていません"), *ArgName);
		if (bIsValidArg)
		{
			const FArgInfo& ArgInfo = ArgInfos[ArgName];
			const bool bIsValidArgType = ArgInfo.GetValidateType() == EType::None ||
				RequiredType == EType::None ||
				ArgInfo.GetValidateType() == RequiredType;

			ensureAlwaysMsgf(bIsValidArgType, TEXT("引数 %s が求められている型と一致しません"), *ArgName);
			return bIsValidArgType;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}


bool FArgParser::GetValue(const FString& ArgName, int8& Value) const
{
	return GetValueInteger<int8>(ArgName, Value);
}


bool FArgParser::GetValue(const FString& ArgName, int16& Value) const
{
	return GetValueInteger<int16>(ArgName, Value);
}

bool FArgParser::GetValue(const FString& ArgName, int32& Value) const
{
	return GetValueInteger<int32>(ArgName, Value);
}

bool FArgParser::GetValue(const FString& ArgName, int64& Value) const
{
	if (IsValidArg(ArgName, EType::Integer))
	{
		Value = FCString::Atoi64(*ArgInfos[ArgName].GetParsedValue());
		return true;
	}
	else
	{
		return false;
	}
}

bool FArgParser::GetValue(const FString& ArgName, float& Value) const
{
	if (IsValidArg(ArgName, EType::Float))
	{
		Value = FCString::Atof(*ArgInfos[ArgName].GetParsedValue());
		return true;
	}
	else
	{
		return false;
	}
}

bool FArgParser::GetValue(const FString& ArgName, FString& Value) const
{
	if (IsValidArg(ArgName, EType::None))
	{
		Value = ArgInfos[ArgName].GetParsedValue();
		return true;
	}
	else
	{
		return false;
	}
}


bool FArgParser::GetValue(const FString& ArgName, bool& Value) const
{
	if (IsValidArg(ArgName, EType::Bool))
	{
		Value = ArgInfos[ArgName].GetParsedValue().ToUpper() == TEXT("TRUE");
		return true;
	}
	else
	{
		return false;
	}
}

bool FArgParser::GetValue(const FString& ArgName, FVector& Value) const
{
	if (IsValidArg(ArgName, EType::Vector))
	{
		return Value.InitFromString(ArgInfos[ArgName].GetParsedValue());
	}
	else
	{
		return false;
	}
}


/**
 * @brief 引数情報
 */
/*static*/
TSharedPtr<FArgParser::FArgInfo::FRegexData> FArgParser::FArgInfo::RegexData;


FArgParser::FArgInfo::FArgInfo(EType ValidateType, bool bRequired)
	: ValidateType(ValidateType), bRequired(bRequired)
{
}

bool FArgParser::FArgInfo::Parse(const FString& ArgName, const FString& Command)
{
	if (!RegexData.IsValid())
	{
		RegexData = MakeShareable(new FRegexData());
	}

	if (!ensureAlways(!ParsedValue.IsSet()))
	{
		return false;
	}

	// 引数の値が入っている文字列を取得
	const FString SearchArgName = FString::Printf(TEXT(" %s"), *ArgName);
	const int32 Index = Command.Find(SearchArgName);
	FString LeftStr, RightStr;
	if (Command.Split(ArgName, &LeftStr, &RightStr))
	{
		// 引数の値が入った文字列の先頭の空白を取り除く
		const int32 RightChopCount = FCString::Strspn(*RightStr, TEXT(" \r\n\t"));
		RightStr.RightChopInline(RightChopCount);

		// 引数の値を取り出す
		// ""でくくられている場合はその間を値として取り出す
		// くくられていなければ空白するまでの間を取り出す

		FRegexMatcher QuotationMatch(RegexData->QuotationValuePattern, RightStr);
		FRegexMatcher NoQuotationMatch(RegexData->NoQuotationPattern, RightStr);
		if (QuotationMatch.FindNext())
		{
			ParsedValue = QuotationMatch.GetCaptureGroup(1);
			return ValidateArgType(ArgName, ParsedValue.GetValue(), ValidateType);
		}
		else if (NoQuotationMatch.FindNext())
		{
			ParsedValue = NoQuotationMatch.GetCaptureGroup(1);
			return ValidateArgType(ArgName, ParsedValue.GetValue(), ValidateType);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("引数 %s をパースできませんでした。コマンド:%s"), *ArgName, *Command);
			return false;
		}
	}
	else
	{
		// 見つからなかった場合、必須引数であればエラーとする
		if (bRequired)
		{
			UE_LOG(LogTemp, Error, TEXT("必須引数 %s が存在しません。コマンド:%s"), *ArgName, *Command);
			return false;
		}
		else
		{
			return true;
		}
	}
}

void FArgParser::FArgInfo::Reset()
{
	ParsedValue.Reset();
}

bool FArgParser::FArgInfo::IsParsed() const
{
	return ParsedValue.IsSet();
}

const FString& FArgParser::FArgInfo::GetParsedValue() const
{
	ensureAlways(IsParsed());
	return ParsedValue.GetValue();
}

FArgParser::EType FArgParser::FArgInfo::GetValidateType() const
{
	return ValidateType;
}

bool FArgParser::FArgInfo::ValidateArgType(const FString& ArgName, const FString& ArgValue, EType validateType)
{
	switch (validateType)
	{
	case EType::None:
		return true;
	case EType::Bool:
		if (ArgValue.ToUpper() == TEXT("TRUE"))
		{
			return true;
		}
		else if (ArgValue.ToUpper() == TEXT("FALSE"))
		{
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("引数 %s の値 %s はbool値ではありません"), *ArgName, *ArgValue);
			return false;
		}
	case EType::Float:
		{
			FRegexMatcher FloatMatch(RegexData->FloatPattern, ArgValue);
			if (FloatMatch.FindNext())
			{
				return true;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("引数 %s の値 %s は浮動小数値ではありません"), *ArgName, *ArgValue);
				return false;
			}
		}
	case EType::Integer:
		{
			FRegexMatcher FloatMatch(RegexData->IntegerPattern, ArgValue);
			if (FloatMatch.FindNext())
			{
				return true;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("引数 %s の値 %s は整数値ではありません"), *ArgName, *ArgValue);
				return false;
			}
		}
	case EType::Vector:
		{
			FVector Vec;
			if (Vec.InitFromString(ArgValue))
			{
				return true;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("引数 %s の値 %s はFVector値ではありません"), *ArgName, *ArgValue);
				return false;
			}
		}
	default:
		ensureAlwaysMsgf(false, TEXT("不正な型です"));
		return false;
	}
}
