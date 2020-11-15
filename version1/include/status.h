#pragma once
namespace zhtcp{
namespace server{//各种状态
class status  //
   {  public: 
       enum LINE_STATUS { LINE_OK = 0, LINE_BAD, LINE_MORE };
       enum CHECK_STATE { PARSE_REQUESTLINE = 0, PARSE_HEADER, PARSE_BODY };
       enum HTTP_CODE { NO_REQUEST, GET_REQUEST, BAD_REQUEST, FORBIDDEN_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION };
   };
   class request
   {
       public:
        enum HTTP_VERSION { HTTP_10 = 0, HTTP_11, VERSION_NOT_SUPPORT};
        enum HTTP_METHOD { GET = 0, POST, PUT, DELETE, METHOD_NOT_SUPPORT};
        enum HTTP_HEADER { Host = 0, User_Agent, Connection, Accept_Encoding, Accept_Language, Accept, Cache_Control, Upgrade_Insecure_Requests};
   };
   class response
   {
       public:
          enum HttpStatusCode 
          {
            Unknow,
            k200Ok = 200,
            k403forbiden = 403,
            k404NotFound = 404
        };
   };
   
}
}