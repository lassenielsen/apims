(nu rnd: 2=>1<Bool>;Gend)
( def Rnd() = // {{{
    link(2,rnd,s,2);
    ( Rnd()
    | (nu a: {^TRUE: Gend, ^FALSE: Gend})
      link(1,a,select,1);
      sync(1,select)
      {^TRUE: s[1]<<true;end,
       ^FALSE: s[1]<<false;end
      }
    )
  in Rnd() | // }}}
(nu rand: 1=>2<Int>;2=>1<Int>;Gend)
( def Rand() = // {{{
    link(2,rand,s,2);
    ( Rand()
    | def Random(min: Int, max: Int, dest: 1<<<Int>;Lend@(2of2)) =
        if max <= min
        then dest[1]<<min; end
        else link(2,rnd,select,1);
             select[2]>>choice;
             if choice
             then Random(min,(min+max)/2,dest)
             else Random((min+max)/2+1,max,dest)
      in
        s[1]>>max;
        Random(0,max-1,s)
    )
  in Rand() | // }}}
(nu mod: 1=>2<Int>;
         1=>2<Int>;
         2=>1<Int>;
         Gend)
( def ModService() = // {{{
    link(2,mod,m,2);
    ( ModService()
    | m[1]>>n;
      m[1]>>k;
      m[1]<<n-((n/k)*k);
      end
    )
  in ModService() | // }}}
(nu exp: 1=>2<Int>;
         1=>2<Int>;
         2=>1<Int>;
         Gend)
( def ExpService() = // {{{
    link(2,exp,e,2);
    ( ExpService()
    | e[1]>>n;
      e[1]>>k;
      def Exp(k: Int,acc: Int,res: Int, dest: 1<<<Int>;Lend@(2of2)) =
        if k<=0
        then dest[1]<<res;
             end
        else link(2,mod,mod1,1);
             mod1[2]<<k;
             mod1[2]<<2;
             mod1[2]>>rest;
             if rest <=0
             then Exp(k/2,acc*acc,res,dest)
             else Exp(k/2,acc*acc,res*acc,dest)
      in Exp(k,n,1,e)
    )
  in ExpService() | // }}}
(nu expmod: 1=>2<Int>;
            1=>2<Int>;
            1=>2<Int>;
            2=>1<Int>;
            Gend)
( def ExpModService() = // {{{
    link(2,expmod,e,2);
    ( ExpModService()
    | e[1]>>n;
      e[1]>>k;
      e[1]>>m;
      def ExpMod(k: Int,acc: Int,res: Int, dest: 1<<<Int>;Lend@(2of2)) =
        if k<=0
        then dest[1]<<res;
             end
        else link(2,mod,mod1,1);
             mod1[2]<<k;
             mod1[2]<<2;
             mod1[2]>>rest;
             link(2,mod,mod2,1);
             mod2[2]<<acc*acc;
             mod2[2]<<m;
             mod2[2]>>newacc;
             if rest <=0
             then ExpMod(k/2,newacc,res,dest)
             else link(2,mod,mod3,1);
                  mod3[2]<<res*acc;
                  mod3[2]<<m;
                  mod3[2]>>newres;
                  ExpMod(k/2,newacc,newres,dest)
      in ExpMod(k,n,1,e)
    )
  in ExpModService() | // }}}
(nu gcd: 1=>2<Int>;
         1=>2<Int>;
         2=>1<Int>;
         Gend)
( def GCDService() = // {{{
    link(2,gcd,s,2);
    ( GCDService()
    | def GCD(n: Int,m: Int, dest: 1<<<Int>;Lend@(2of2)) =
        if m<=0
        then dest[1]<<n;
             end
        else link(2,mod,mod1,1);
             mod1[2]<<n;
             mod1[2]<<m;
             mod1[2]>>rest;
             GCD(m,rest,dest)
      in
        s[1]>>n;
        s[1]>>m;
        GCD(n,m,s)
    )
  in GCDService() | // }}}
(nu rsa_makeprime: 2=>1<Int>;
                   Gend)
( def RSA_MakePrimeService() = // {{{
//    link(2,rsa_makeprime,mp,2);
//    mp[1]<<733;
//    link(2,rsa_makeprime,mp,2);
//    mp[1]<<1093;
    link(2,rsa_makeprime,mp,2);
    ( RSA_MakePrimeService()
    | def RSA_MakePrime(dest: 1<<<Int>;Lend@(2of2)) =
        // Create random number (guess prime)
        link(2,rand,rand1,1);
        rand1[2]<<1024;
        rand1[2]>>p;
        def CheckPrime(checks: Int, dest: 1<<<Int>;Lend@(2of2)) =
          if checks<=0
          then dest[1]<<p;
	       end
	  else // Create random numbers to find factors
	       link(2,rand,rand1,1);
	       rand1[2]<<(2*p)-1;
	       rand1[2]>>r;
               link(2,expmod,em,1);
               em[2]<<1+r;
               em[2]<<2*p;
               em[2]<<(2*p)+1;
               em[2]>>x1;
	       if x1=1
	       then CheckPrime(checks-1,dest)
	       else //log<<p;
                    RSA_MakePrime(dest)
	in CheckPrime(15,dest)
      in RSA_MakePrime(mp)
    )
  in RSA_MakePrimeService() | // }}}
define $rsa_newkey = // {{{
  1=>2<Int>;
  1=>2<Int>;
  2=>1<Int>;
  2=>1<Int>;
  2=>1<Int>;
  Gend
in // }}}
(nu rsa_newkey: $rsa_newkey)
( def RSA_NewKeyService() = // {{{
    link(2,rsa_newkey,nk,2);
    ( RSA_NewKeyService()
    | nk[1]>>p1;
      nk[1]>>p2;
      nk[1]<<p1*p2;
      link(2,rand,r,1);
      r[2]<<(p1-1)*(p2-1)/2;
      r[2]>>k1;
      link(2,gcd,gcd1,1);
      gcd1[2]<<((p1-1)*(p2-1))+1;
      gcd1[2]<<(((p1-1)*(p2-1))/2)+k1;
      gcd1[2]>>key1;
      nk[1]<<key1;
      nk[1]<<(((p1-1)*(p2-1))+1)/key1;
      end
    )
  in
    RSA_NewKeyService() | // }}}
define $rsa = // {{{
  rec $rsa.
  1=>2
  {^Code:
    1=>2<Int>;
    2=>1<Int>;
    $rsa,
   ^Delete:
    Gend
  }
in // }}}
(nu rsa: 1=>2<Int>; 1=>2<Int>; $rsa)
( def RSA_Service() = // {{{
    link(2,rsa,s,2);
    ( RSA_Service()
    | s[1]>>n;
      s[1]>>d;
      def Encode(s: $rsa@(2of2)) =
        s[1]>>
        {^Code:
          s[1]>>val;
          link(2,expmod,exp1,1);
          exp1[2]<<val;
          exp1[2]<<d;
          exp1[2]<<n;
          exp1[2]>>newval;
          s[1]<<newval;
//          (nu log: Gend)
//          log<<n;
//          log<<d;
//          log<<val;
//          log<<newval;
          Encode(s),
         ^Delete:
          end
        }
      in Encode(s)
    )
  in RSA_Service() | // }}}
// Create two primes
link(2,rsa_makeprime,mp1,1);
mp1[2]>>p1;
link(2,rsa_makeprime,mp2,1);
mp2[2]>>p2;
// Create public/private key pair
link(2,rsa_newkey,nk,1);
nk[2]<<p1;
nk[2]<<p2;
nk[2]>>n;
nk[2]>>priv;
nk[2]>>pub;
// Create Encryption object
link(2,rsa,encoder,1);
encoder[2]<<n;
encoder[2]<<pub;
// Create Decryption object
link(2,rsa,decoder,1);
decoder[2]<<n;
decoder[2]<<priv;
// Time to do some encoding.
encoder[2]<<^Code;
encoder[2]<<12345;
encoder[2]>>code;
decoder[2]<<^Code;
decoder[2]<<code;
decoder[2]>>x12345;
// Clean up
encoder[2]<<^Delete;
decoder[2]<<^Delete;
end
) ) ) ) ) ) ) ) )
