module power_profile
use power_type
use power_input_parameter
implicit none

type profile_type
    integer(kind=ik),dimension(:),allocatable :: id
    real(kind=rtk),dimension(:),allocatable :: start_sec
    real(kind=rtk),dimension(:),allocatable :: start_microsec
    real(kind=rtk),dimension(:),allocatable :: start_nanosec
    real(kind=rtk),dimension(:),allocatable :: end_sec
    real(kind=rtk),dimension(:),allocatable :: end_microsec
    real(kind=rtk),dimension(:),allocatable :: end_nanosec
    real(kind=rtk)                          :: offset_sec
    real(kind=rtk)                          :: offset_microsec
    real(kind=rtk)                          :: offset_nanosec
    integer(kind=ik)                        :: num_profiles
end type profile_type

contains


subroutine power_profile_new(length,profiles)
  integer(kind=ik),intent(in)       :: length
  type(profile_type),intent(out)    :: profiles
  
  allocate(profiles%id(length))
  allocate(profiles%start_sec(length))
  allocate(profiles%start_microsec(length))
  allocate(profiles%start_nanosec(length))
  allocate(profiles%end_sec(length))
  allocate(profiles%end_microsec(length))
  allocate(profiles%end_nanosec(length))
  profiles%id(:) = 0_ik
  profiles%start_sec(:) = 0.0_rtk
  profiles%start_nanosec(:) = 0.0_rtk
  profiles%end_sec(:) = 0.0_rtk
  profiles%end_nanosec(:) = 0.0_rtk
  profiles%start_microsec(:) = 0.0_rk
  profiles%end_microsec(:) = 0.0_rk
  
  profiles%num_profiles = 0_ik
end subroutine  power_profile_new

subroutine power_read_profile_from_file(input_parameters, profiles)
   type(power_input_parameter_type) ,intent(in)     :: input_parameters !user input
   type(profile_type),intent(out)                   :: profiles
   integer(kind=ik)                                 :: num_colums, ii, unit_nr, error_code 
   integer(kind=ik) :: num_lines
   character(len=str_length)                        :: line_read
   character(len=str_length)                        :: filepath
   
   write(filepath,'(A,A,A)') trim(input_parameters%profile_topdir),trim('/'),&
   trim(input_parameters%profile_file)
   profiles%num_profiles = 0_ik
   num_lines=0_ik
   num_colums=4_ik
   unit_nr = 1000_ik
   error_code = 0_ik
  !to get the number of lines
   open(unit = unit_nr, file = filepath, status = 'old', action = 'read')
    do
     read(unit_nr,*, end=10) line_read
     num_lines=num_lines+1
    end do
  10 close(unit_nr)
  open(unit = unit_nr, file = filepath, status = 'old', action = 'read', position= 'rewind', iostat = error_code)
  if(error_code .ne. 0_ik) then
    write(*,'(A,I0)')  "Error open:",error_code
    stop
  end if
   num_lines=num_lines-1
   if(num_lines .gt. 0) then
    !the first line is ignored
     call power_profile_new(num_lines,profiles)
    write(*,'(A,I0)') "line:",num_lines
     !read line by line and get the start and end time
     do ii=1_ik, num_lines
      if(ii .eq. 1_ik) then
       read(unit_nr,*) line_read
      end if
      read(unit_nr,*) profiles%id(ii),profiles%start_sec(ii),&
        profiles%start_nanosec(ii),profiles%end_sec(ii),&
        profiles%end_nanosec(ii)

     end do
     close(unit_nr)
     profiles%num_profiles = num_lines
   end if
 end subroutine  power_read_profile_from_file
 
end module power_profile
