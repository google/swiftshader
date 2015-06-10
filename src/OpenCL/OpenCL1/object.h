//TODO: copyrights

#ifndef __REFCOUNTED_H__
#define __REFCOUNTED_H__

#include <list>

namespace Devices
{

	/**
	* \brief Base class of all the Clover objects
	*
	* This class implements functions needed by all the Clover objects, like
	* reference counting, the object tree (parents/children), etc.
	*
	* It also uses a special list of known objects, used to check that a pointer
	* passed by the user to an OpenCL function actually is an object of the correct
	* type. See \c isA().
	*/
	class Object
	{
	public:
		/**
		* \brief Type of object the inherited class actually is
		*/
		enum Type
		{
			T_Device,       /*!< \brief \c Coal::DeviceInterface */
			T_CommandQueue, /*!< \brief \c Coal::CommandQueue */
			T_Event,        /*!< \brief \c Coal::Event */
			T_Context,      /*!< \brief \c Coal::Context */
			T_Kernel,       /*!< \brief \c Coal::Kernel */
			T_MemObject,    /*!< \brief \c Coal::MemObject */
			T_Program,      /*!< \brief \c Coal::Program */
			T_Sampler       /*!< \brief \c Coal::Sampler */
		};

		/**
		* \brief Constructor
		* \param type type of the child class calling this constructor
		* \param parent parent object
		*/
		Object(Type type, Object *parent = 0);
		virtual ~Object();

		/**
		* \brief Increments the reference counter
		*/
		void reference();

		/**
		* \brief Decrements the reference counter
		* \return true if the reference counter has reached 0
		*/
		bool dereference();

		/**
		* \brief Reference counter
		* \return the number of references of this class currently in use
		*/
		unsigned int references() const;

		/**
		* \brief Set if the parent object has to be deleted if its reference count reaches 0
		*
		* The destructor of \c Coal::Object dereferences its parent object.
		* This is done in order to correctly free objects when no object has
		* a reference to it anymore.
		*
		* Some objects such as \c Coal::CommandQueue need to do some operations
		* before being deleted. This function tells \c Coal::Object to
		* dereference its parent object, but not to call \b delete on it.
		*
		* \param release true to have \b delete called on the parent object
		*        when its reference count reaches 0, false to keep it
		*/
		void setReleaseParent(bool release);

		Object *parent() const;    /*!< \brief Parent object */
		Type type() const;         /*!< \brief Type */

		/**
		* \brief Returns whether this object is an instance of \p type
		* \note This function begins with a NULL-check on the \c this pointer,
		*       so it's safe to use even when \c this is not guaranteed not to
		*       be NULL.
		* \param type type this object must have for the check to pass
		* \return true if this object exists and has the correct type
		*/
		bool isA(Type type) const;

	private:
		unsigned int p_references;
		Object *p_parent;
		Type p_type;
		std::list<Object *>::iterator p_it;
		bool p_release_parent;
	};

}
#endif