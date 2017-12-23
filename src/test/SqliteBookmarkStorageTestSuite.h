#include "cxxtest/TestSuite.h"

#include "boost/filesystem.hpp"

#include "data/storage/sqlite/SqliteBookmarkStorage.h"

class SqliteBookmarkStorageTestSuite: public CxxTest::TestSuite
{
public:
	void test_add_bookmarks()
	{
		FilePath databasePath("data/SQLiteTestSuite/bookmarkTest.sqlite");
		size_t bookmarkCount = 4;
		int result = -1;
		{
			FileSystem::remove(databasePath);
			SqliteBookmarkStorage storage(databasePath);
			storage.setup();

			for (size_t i = 0; i < bookmarkCount; i++)
			{
				const Id categoryId = storage.addBookmarkCategory(StorageBookmarkCategoryData("test category")).id;
				storage.addBookmark(StorageBookmarkData("test bookmark", "test comment", TimeStamp::now().toString(), categoryId));
			}

			result = storage.getAllBookmarks().size();
		}

		FileSystem::remove(databasePath);

		TS_ASSERT_EQUALS(result, bookmarkCount);
	}

	void test_add_bookmarked_node()
	{
		FilePath databasePath("data/SQLiteTestSuite/bookmarkTest.sqlite");
		size_t bookmarkCount = 4;
		int result = -1;
		{
			FileSystem::remove(databasePath);
			SqliteBookmarkStorage storage(databasePath);
			storage.setup();

			const Id categoryId = storage.addBookmarkCategory(StorageBookmarkCategoryData("test category")).id;
			const Id bookmarkId = storage.addBookmark(StorageBookmarkData("test bookmark", "test comment", TimeStamp::now().toString(), categoryId)).id;

			for (size_t i = 0; i < bookmarkCount; i++)
			{
				storage.addBookmarkedNode(StorageBookmarkedNodeData(bookmarkId, "test name"));
			}

			result = storage.getAllBookmarkedNodes().size();
		}

		FileSystem::remove(databasePath);

		TS_ASSERT_EQUALS(result, bookmarkCount);
	}

	void test_remove_bookmark_also_removes_bookmarked_node()
	{
		FilePath databasePath("data/SQLiteTestSuite/bookmarkTest.sqlite");
		int result = -1;
		{
			FileSystem::remove(databasePath);
			SqliteBookmarkStorage storage(databasePath);
			storage.setup();

			const Id categoryId = storage.addBookmarkCategory(StorageBookmarkCategoryData("test category")).id;
			const Id bookmarkId = storage.addBookmark(StorageBookmarkData("test bookmark", "test comment", TimeStamp::now().toString(), categoryId)).id;
			storage.addBookmarkedNode(StorageBookmarkedNodeData(bookmarkId, "test name"));

			storage.removeBookmark(bookmarkId);

			result = storage.getAllBookmarkedNodes().size();
		}

		FileSystem::remove(databasePath);

		TS_ASSERT_EQUALS(result, 0);
	}

	void test_edit_nodeBookmark()
	{
		FilePath databasePath("data/SQLiteTestSuite/bookmarkTest.sqlite");

		const std::string updatedName = "updated name";
		const std::string updatedComment = "updated comment";

		StorageBookmark storageBookmark;
		{
			FileSystem::remove(databasePath);
			SqliteBookmarkStorage storage(databasePath);
			storage.setup();

			const Id categoryId = storage.addBookmarkCategory(StorageBookmarkCategoryData("test category")).id;
			const Id bookmarkId = storage.addBookmark(StorageBookmarkData("test bookmark", "test comment", TimeStamp::now().toString(), categoryId)).id;
			storage.addBookmarkedNode(StorageBookmarkedNodeData(bookmarkId, "test name"));

			storage.updateBookmark(bookmarkId, updatedName, updatedComment, categoryId);

			storageBookmark = storage.getAllBookmarks().front();
		}

		FileSystem::remove(databasePath);

		TS_ASSERT_EQUALS(updatedName, storageBookmark.name);
		TS_ASSERT_EQUALS(updatedComment, storageBookmark.comment);
	}
};
