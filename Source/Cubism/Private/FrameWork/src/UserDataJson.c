/*
* Copyright(c) Live2D Inc. All rights reserved.
*
* Use of this source code is governed by the Live2D Open Software license
* that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
*/


// -------- //
// REQUIRES //
// -------- //

#include "Local.h"

#include <Live2DCubismFrameworkINTERNAL.h>


// ----- //
// TYPES //
// ----- //

/// Parser state.
typedef enum ParserState
{
  /// Common flag signalizing parsing hasn't actually started yet.
  Pending,

  /// Common flag signalizing parser is waiting for something...
  Waiting,

  /// Common flag signaling parsing has finished.
  FinishedParsing,


  /// Flag for version parser to read version.
  ReadingVersion,


  ReadingUserDataCount,

  ReadingTotalUserDataSize,


  ReadingUserData,

  ReadingTarget,

  ReadingId,

  ReadingValue,
}
ParserState;

/// Context for version parser.
typedef struct VersionParserContext
{
  /// Parser state.
  ParserState State;

  /// Buffer to write to.
  int* Buffer;
}
VersionParserContext;

/// Context for meta data parser.
typedef struct MetaParserContext
{
  /// Parser state.
  ParserState State;

  /// Buffer to write to.
  UserDataJsonMeta* Buffer;
}
MetaParserContext;

/// Context for user data parser.
typedef struct UserDataParserContext
{
  /// Parser state.
  ParserState State;

  /// Buffer to write to.
  csmUserData* Buffer;


  // TODO Document
  UserDataJsonMeta Meta;

  // TODO Document
  int TagIndex;

  // TODO Document
  int ValueIndex;
}
UserDataParserContext;

// --------------------------- //
// VERSION INDEPENDENT PARSERS //
// --------------------------- //

/// Initializes context.
///
/// @param  context  Context to initialize.
/// @param  buffer   Buffer to write to.
static void InitializeVersionParserContext(VersionParserContext* context, int* buffer)
{
  context->State = Pending;
  context->Buffer = buffer;
}


static int ParseVersion(const char* jsonString, csmJsonTokenType type, int begin, int end, void* versionParserContext)
{
  VersionParserContext* context;


  // Recover context.
  context = versionParserContext;


  // Handle token.
  switch (context->State)
  {
    // Wait for version section...
  case Pending:
  {
    // Skip non-names.
    if (type != csmJsonName)
    {
      ;
    }


    else if (DoesStringStartWith(jsonString + begin, "Version"))
    {
      context->State = ReadingVersion;
    }


    break;
  }


  // ... then read version.
  case ReadingVersion:
  {
    ReadIntFromString(jsonString + begin, context->Buffer);


    context->State = FinishedParsing;


    break;
  }


  default:
  {
    break;
  }
  }


  // Stop lexing as soon as version read.
  return context->State != FinishedParsing;
}


/// Initializes context.
///
/// @param  context  Context to initialize.
/// @param  buffer   Buffer to write to.
static void InitializeMetaParserContext(MetaParserContext* context, UserDataJsonMeta* buffer)
{
  context->State = Pending;
  context->Buffer = buffer;
}

// TODO Document
static void InitializeUserDataParserContext(UserDataParserContext* context, csmUserData* buffer)
{
  context->State = Pending;
  context->Buffer = buffer;
  context->TagIndex = 0;
  context->ValueIndex = 0;


  // 'Flag' buffer as uninitialized...
  context->Buffer->Tags = 0;
}

static int ParseMeta3(const char* jsonString, csmJsonTokenType type, int begin, int end, void* metaParserContext)
{
  MetaParserContext* context;


  // Recover context.
  context = metaParserContext;


  // Parse data.
  switch (context->State)
  {
    // Wait for meta section.
  case Pending:
  {
    // Skip non-names.
    if (type != csmJsonName)
    {
      ;
    }


    else if (DoesStringStartWith(jsonString + begin, "Meta"))
    {
      context->State = Waiting;
    }


    break;
  }


  // Look for data to read.
  case Waiting:
  {
    // Stop parsing if end of meta section reached.
    if (type == csmJsonObjectEnd)
    {
      context->State = FinishedParsing;
    }


    // Select data to read.
    else
    {
      if (DoesStringStartWith(jsonString + begin, "UserDataCount"))
      {
        context->State = ReadingUserDataCount;
      }
      else if (DoesStringStartWith(jsonString + begin, "TotalUserDataSize"))
      {
        context->State = ReadingTotalUserDataSize;
      }
    }


    break;
  }


  // Read number of user data.
  case ReadingUserDataCount:
  {
    ReadIntFromString(jsonString + begin, &context->Buffer->UserDataCount);

    context->State = Waiting;


    break;
  }


  // Read size of total user data in bytes.
  case ReadingTotalUserDataSize:
  {
    ReadIntFromString(jsonString + begin, &context->Buffer->TotalUserDataSize);

    context->Buffer->TotalUserDataSize += context->Buffer->UserDataCount;

    context->State = Waiting;


    break;
  }


  default:
  {
    break;
  }
  }


  // Stop lexing as soon as version read.
  return context->State != FinishedParsing;
}


static int ParseUserData3(const char* jsonString, csmJsonTokenType type, int begin, int end, void* userDataParserContext)
{
  MetaParserContext metaParserContext;
  UserDataParserContext* context;
  int i;
  short escaped;


  // Recover context.
  context = userDataParserContext;


  // Initialize meta related fields if necessary.
  if (!context->Buffer->Tags)
  {
    // Parse meta.
    InitializeMetaParserContext(&metaParserContext, &context->Meta);
    csmLexJson(jsonString, ParseMeta3, &metaParserContext);


    // Initialize data fields.
    context->Buffer->TagCount = context->Meta.UserDataCount;


    // Initialize pointer fields.
    context->Buffer->Tags = (csmUserDataTag*)(context->Buffer + 1);
    context->Buffer->Values = (char*)(context->Buffer->Tags + context->Meta.UserDataCount);
  }


  // Handle token.
  switch (context->State)
  {
    // Wait for curves section.
    case Pending:
    {
      // Skip non-names.
      if (type != csmJsonName)
      {
        ;
      }


      else if (DoesStringStartWith(jsonString + begin, "UserData"))
      {
        context->State = Waiting;
      }


      break;
    }


    // Start reading curve or finalize parsing.
    case Waiting:
    {
      // Stop parsing at last curve.
      if (type == csmJsonArrayEnd)
      {
        context->State = FinishedParsing;
      }


      // Start parsing curve.
      else if (type == csmJsonObjectBegin)
      {
        // Initialize curve fields.
        context->Buffer->Tags[context->TagIndex].BaseValueIndex = context->ValueIndex;


        // Prepare context.
        context->State = ReadingUserData;
      }


      break;
    }


    // Handle parsing of a single curve.
    case ReadingUserData:
    {
      // End curve parsing.
      if (type == csmJsonObjectEnd)
      {
        context->Buffer->Tags[context->TagIndex].ValueCount = context->ValueIndex - context->Buffer->Tags[context->TagIndex].BaseValueIndex;


        // Update context.
        context->TagIndex += 1;

        context->State = Waiting;
      }


      // Prepare context for reading curve data.
      else
      {
        if (DoesStringStartWith(jsonString + begin, "Target"))
        {
          context->State = ReadingTarget;
        }
        else if (DoesStringStartWith(jsonString + begin, "Id"))
        {
          context->State = ReadingId;
        }
        else if (DoesStringStartWith(jsonString + begin, "Value"))
        {
          context->State = ReadingValue;
        }
      }


      break;
    }


    // Read curve target.
    case ReadingTarget:
    {
      if (DoesStringStartWith(jsonString + begin, "ArtMesh"))
      {
        context->Buffer->Tags[context->TagIndex].Type = csmArtMeshUserData;
      }


      // Allow parsing of other curve data.
      context->State = ReadingUserData;


      break;
    }


    // Read curve id.
    case ReadingId:
    {
      // Hash ID..
      context->Buffer->Tags[context->TagIndex].Id = csmHashIdFromSubString(jsonString, begin, end);


      // Allow parsing of other curve data.
      context->State = ReadingUserData;


      break;
    }


    // Read curve segments.
    case ReadingValue:
    {
      escaped = 0;


      for (i = begin; i < end; ++i)
      {
        if (escaped)
        {
          switch (jsonString[i])
          {
            case 't':
              {
                context->Buffer->Values[context->ValueIndex] = '\t';
                
                break;
              }


            case 'r':
              {
                context->Buffer->Values[context->ValueIndex] = '\r';


                break;
              }


            case 'n':
              {
                context->Buffer->Values[context->ValueIndex] = '\n';


                break;
              }

            case '\"':
            case '\\':
              {
                context->Buffer->Values[context->ValueIndex] = jsonString[i];


                break;
              }


            default:
            {
              break;
            }
          }
          
          
          escaped = 0;
          context->ValueIndex += 1;
        }
        else
        {
          escaped |= jsonString[i] == '\\' ? 1 : 0;


          if (!escaped)
          {
            context->Buffer->Values[context->ValueIndex] = jsonString[i];
            context->ValueIndex += 1;
          }
        }
      }

      context->Buffer->Values[context->ValueIndex] = 0;
      context->ValueIndex += 1;


      // Allow parsing of other curve data.
      context->State = ReadingUserData;


      break;
    }


    default:
    {
      break;
    }
  }


  // Stop lexing as soon as version read.
  return context->State != FinishedParsing;
}


// ---------------------- //
// USER DATA JSON PARSING //
// ---------------------- //

/// Available physics meta readers.
static csmJsonTokenHandler MetaParsers[] =
{
  0,
  0,
  0,
  ParseMeta3
};

/// Available physics readers.
static csmJsonTokenHandler UserDataParsers[] =
{
  0,
  0,
  0,
  ParseUserData3,
};


void ReadUserDataJsonMeta(const char* userDataJson, UserDataJsonMeta* buffer)
{
  VersionParserContext versionParserContext;
  MetaParserContext context;
  int version;


  // Get version info.
  InitializeVersionParserContext(&versionParserContext, &version);
  csmLexJson(userDataJson, ParseVersion, &versionParserContext);


  // Parse matching version.
  InitializeMetaParserContext(&context, buffer);
  csmLexJson(userDataJson, MetaParsers[version], &context);
}

void ReadUserDataJson(const char* userDataJson, csmUserData* buffer)
{
  VersionParserContext versionParserContext;
  UserDataParserContext context;
  int version;


  // Get version info.
  InitializeVersionParserContext(&versionParserContext, &version);
  csmLexJson(userDataJson, ParseVersion, &versionParserContext);


  // Parse matching version.
  InitializeUserDataParserContext(&context, buffer);
  csmLexJson(userDataJson, UserDataParsers[version], &context);
}
