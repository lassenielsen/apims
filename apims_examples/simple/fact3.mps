(nu fact : 1=>2:1<Int>;2=>1:2<Int>;Gend) // Fact Server protocol
(nu cont : 1=>2:1<Int>;                  // Fact Continuation protocol
           1=>2:1<Int>;
           1=>2:1<2<<<Int>;Lend@(2of2)>;
           Gend)
( def FactCont() =                       // Fact Continuation Server
      link(2,cont,s,2);                  // Receive Connection
      ( FactCont()                       // Start new Server
      | s[1] >> x;                       // Receive argument (x)
        s[1] >> factor;                  // Receive (x+1)*...*n
        s[1] >> client;                  // Receive Client Session
        if x=1                           // If simple case
        then client[2] << factor; end    // Return directly
        else link(2,cont,q,1);           // Connect to (another) Cont Server
             q[1] << x-1;                // Send x-1
             q[1] << x*factor;           // Send x*(x+1)*...*n
             q[1] << client;             // Send Client Session
             end
      ) in FactCont()                    // Start Continuation Server
| def Fact() =                           // Define Fact Server
      link(2,fact,s,2);                  // Receive Connection
      ( Fact()                           // Start new Server
      | link(2,cont,t,1);                // Connect to Cont. Server
        s[1]>>n;                         // Receive n
        t[1]<<n;t[1]<<1;t[1]<<s;         // Delegate to Cont
        end
      ) in Fact()                        // Start Fact Server
| (nu gui: {^Fact: {^Quit: Gend} })      // User Interface Behavior
  ( link(1,gui,user,1);                  // Create User Interface
    guisync(1,user,1)                    // Wait for User Input
    {^Fact(n: Int=5):                    // Request n
      link(2,fact,s,1);                  // Connect to Fact Server
      s[1] << n;                         // Send n
      s[2] >> result;                    // Receive n!
      guivalue(1,user,1,"Fact:",result); // Add result to User Interface
      guisync(1,user,1)                  // Wait for user to quit
      {^Quit(comment: String=""): end }
    }
) )
