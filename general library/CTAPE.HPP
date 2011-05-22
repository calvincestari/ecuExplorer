#if ! defined( TAPE_CLASS_HEADER )

#define TAPE_CLASS_HEADER

class CTapeGetDriveParameters : public _TAPE_GET_DRIVE_PARAMETERS
{
   public:

      CTapeGetDriveParameters();
      CTapeGetDriveParameters( const CTapeGetDriveParameters& source );
      CTapeGetDriveParameters( const TAPE_GET_DRIVE_PARAMETERS * source );

      virtual ~CTapeGetDriveParameters();

      virtual void Copy( const CTapeGetDriveParameters& source );
      virtual void Copy( const TAPE_GET_DRIVE_PARAMETERS * source );
      virtual void Empty( void );

#if defined( _DEBUG )

      virtual void Dump( CDumpContext& dump_context ) const;

#endif
};

class CTapeGetMediaParameters : public _TAPE_GET_MEDIA_PARAMETERS
{
   public:

      CTapeGetMediaParameters();
      CTapeGetMediaParameters( const CTapeGetMediaParameters& source );
      CTapeGetMediaParameters( const TAPE_GET_MEDIA_PARAMETERS * source );

      virtual ~CTapeGetMediaParameters();

      virtual void Copy( const CTapeGetMediaParameters& source );
      virtual void Copy( const TAPE_GET_MEDIA_PARAMETERS * source );
      virtual void Empty( void );

#if defined( _DEBUG )

      virtual void Dump( CDumpContext& dump_context ) const;

#endif
};

class CTapeSetDriveParameters : public _TAPE_SET_DRIVE_PARAMETERS
{
   public:

      CTapeSetDriveParameters();
      CTapeSetDriveParameters( const CTapeSetDriveParameters& source );
      CTapeSetDriveParameters( const TAPE_SET_DRIVE_PARAMETERS * source );

      virtual ~CTapeSetDriveParameters();

      virtual void Copy( const CTapeSetDriveParameters& source );
      virtual void Copy( const TAPE_SET_DRIVE_PARAMETERS * source );
      virtual void Empty( void );

#if defined( _DEBUG )

      virtual void Dump( CDumpContext& dump_context ) const;

#endif
};

class CTapeSetMediaParameters : public _TAPE_SET_MEDIA_PARAMETERS
{
   public:

      CTapeSetMediaParameters();
      CTapeSetMediaParameters( const CTapeSetMediaParameters& source );
      CTapeSetMediaParameters( const TAPE_SET_MEDIA_PARAMETERS * source );

      virtual ~CTapeSetMediaParameters();

      virtual void Copy( const CTapeSetMediaParameters& source );
      virtual void Copy( const TAPE_SET_MEDIA_PARAMETERS * source );
      virtual void Empty( void );

#if defined( _DEBUG )

      virtual void Dump( CDumpContext& dump_context ) const;

#endif
};

class CTape : public CDummyFile
{
   private:

      void m_Initialize( void );

   protected:

      LPVOID m_BackupReadContextPointer;
      LPVOID m_BackupWriteContextPointer;
      
   public:

      CTape();

      virtual ~CTape();

      enum TypeOfPartition
      {
         Fixed     = TAPE_FIXED_PARTITIONS,
         Initiator = TAPE_INITIATOR_PARTITIONS,
         Select    = TAPE_SELECT_PARTITIONS 
      };

      enum TypeOfErasure
      {
         Long  = TAPE_ERASE_LONG,
         Short = TAPE_ERASE_SHORT
      };

      enum TypeOfMark
      {
         File      = TAPE_FILEMARKS,
         LongFile  = TAPE_LONG_FILEMARKS,
         Set       = TAPE_SETMARKS,
         ShortFile = TAPE_SHORT_FILEMARKS
      };

      enum Position
      {
         Absolute = TAPE_ABSOLUTE_POSITION,
         Logical  = TAPE_LOGICAL_POSITION
      };

      enum PositionMethod
      {
         AbsoluteBlock       = TAPE_ABSOLUTE_BLOCK,
         LogicalBlock        = TAPE_LOGICAL_BLOCK,
         Rewind              = TAPE_REWIND,
         EndOfData           = TAPE_SPACE_END_OF_DATA,
         FileMarks           = TAPE_SPACE_FILEMARKS,
         RelativeBlocks      = TAPE_SPACE_RELATIVE_BLOCKS,
         SequentialFileMarks = TAPE_SPACE_SEQUENTIAL_FMKS,
         SequentialSetMarks  = TAPE_SPACE_SEQUENTIAL_SMKS,
         SetMarks            = TAPE_SPACE_SETMARKS
      };

      enum Operation
      {
         Format  = TAPE_FORMAT,
         Load    = TAPE_LOAD,
         Lock    = TAPE_LOCK,
         Tension = TAPE_TENSION,
         Unload  = TAPE_UNLOAD,
         Unlock  = TAPE_UNLOCK
      };

      /*
      ** The Win32 API
      */

      virtual BOOL CreatePartition( const TypeOfPartition type_of_partition,
                                    const DWORD number_of_partitions,
                                    const DWORD number_of_megabytes_in_each_partition ); // CreateTapePartition

      virtual BOOL Erase( const TypeOfErasure type_of_erasure, const BOOL return_immediately = FALSE ); // EraseTape

      virtual BOOL GetParameters( CTapeGetDriveParameters& parameters ); // GetTapeParameters
      virtual BOOL GetParameters( CTapeGetMediaParameters& parameters );

      virtual BOOL GetPosition( const Position type_of_position_to_get,
                                DWORD& partition_number,
                                DWORD& position_low,
                                DWORD& position_high ); // GetTapePosition

      virtual DWORD GetStatus( void ) const; // GetTapeStatus

      virtual BOOL Prepare( const Operation what_to_do, const BOOL return_immediately = FALSE ); // PrepareTape

      virtual BOOL SetParameters( const CTapeSetDriveParameters& parameters ); // SetTapeParameters
      virtual BOOL SetParameters( const CTapeSetMediaParameters& parameters );

      virtual BOOL SetPosition( const PositionMethod how_to_get_there, 
                                const DWORD          partition_number,
                                const DWORD          position_low,
                                const DWORD          position_high,
                                const BOOL           return_immediately = FALSE ); // SetTapePosition

      virtual BOOL Mark( const TypeOfMark type_of_mark, 
                         const DWORD      number_of_marks_to_write, 
                         const BOOL       return_immediately = FALSE ); // WriteTapemark

      virtual BOOL Read( LPBYTE      buffer, 
                         const DWORD number_of_bytes_to_read,
                         LPDWORD     number_of_bytes_read,
                         BOOL        abort = FALSE,
                         BOOL        restore_security_data = TRUE ); // BackupRead

      virtual BOOL Seek( const DWORD seek_low,
                         const DWORD seek_high,
                         LPDWORD seeked_low,
                         LPDWORD seeked_high ); // BackupSeek

      virtual BOOL Write( LPBYTE      buffer,
                          const DWORD number_of_bytes_to_write,
                          LPDWORD     number_of_bytes_written,
                          BOOL        abort = FALSE,
                          BOOL        restore_security_data = TRUE ); // BackupWrite

      /*
      ** API's to make life easier
      */

      virtual BOOL Open( const UINT tape_drive_number_starting_at_zero ); // Calls Prepare( TAPE_LOAD )

      virtual void Close( void ); // Calls Prepare( TAPE_UNLOAD )

      /*
      ** Operators
      */
};

#endif // TAPE_CLASS_HEADER
