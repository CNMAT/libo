
#include <iostream>
#include <vector>

#include "c-access.h"
#include "osc_bundle_u.h"
#include "osc_bundle_s.h"
#include "osc_mem.h"
#include "osc_expr_parser.h"


using namespace std;

int main(int argc, const char * argv[]) {

    vector<t_osc_expr_rec*> rec;
    
    for( int i = 0; i < getNumSignatures(); ++i )
        rec.emplace_back( getRec(i) );
    
    while( 1 )
    {
        for( auto& r : rec )
        {
            t_osc_bndl_u *bndl = osc_bundle_u_alloc();
            
            string sig = osc_expr_rec_getSignature(r);
            
            int n_req_args = osc_expr_rec_getNumRequiredArgs(r);
            int * types = osc_expr_rec_getRequiredArgsTypes(r);
            

            for( int i = 0; i < n_req_args; ++i )
            {
                auto pos = sig.find("$"+to_string(i+1) );
                if(pos != string::npos )
                    sig.replace(pos, 1, "/");
                
                string addr = "/"+to_string(i+1);

                t_osc_msg_u *m = osc_message_u_allocWithAddress( (char *)addr.c_str() );
                osc_message_u_appendAtom(m, generateAtom(types[i]) );
                osc_bundle_u_addMsg(bndl, m);
                
            }
            
            t_osc_bndl_s * s_bndl = osc_bundle_u_serialize(bndl);
            
            
            long len = osc_bundle_s_getLen(s_bndl);
            char *ptr = osc_bundle_s_getPtr(s_bndl);
            
            char *copy = NULL;
            long copylen = len;
            if(strncmp(ptr, "#bundle\0", 8)){
                char alloc = 0;
                osc_bundle_s_wrapMessage(len, ptr, &copylen, &copy, &alloc);
            }else{
                copy = (char *)osc_mem_alloc(len);
                memcpy(copy, ptr, len);
            }
            
            cout << sig << endl;

            t_osc_expr *f = NULL;
            t_osc_err error = osc_expr_parser_parseExpr((char *)sig.c_str(), &f, NULL);
    /*
            if( !error && f )
            {
                t_osc_atom_ar_u *av = NULL;
 
                int ret = osc_expr_eval(f, &copylen, &copy, &av, NULL);
                
                if(av){
                    osc_atom_array_u_free(av);
                }
                
                if( ret )
                {
                    printf("eval error: %s\n", osc_error_string(ret) );
                }
            }
            else
                printf("parse error: %s\n", osc_error_string(error) );
 */
            if( f )
              osc_expr_free(f);


            
            
            osc_bundle_u_free(bndl);
            osc_bundle_s_deepFree(s_bndl);
            osc_mem_free(copy);
            
        }
    }

    
    
    return 0;
}
